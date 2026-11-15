/*
 * tests/test_calc.c - Characterization tests for pricing calculation engine
 */
#include "test_harness.h"
#include "../calc.h"

static void test_rnd2(void)
{
    ASSERT_DOUBLE_EQ(10.55, rnd2(10.554));
    ASSERT_DOUBLE_EQ(10.56, rnd2(10.555));
    ASSERT_DOUBLE_EQ(0.00, rnd2(0.000));
    ASSERT_DOUBLE_EQ(123.45, rnd2(123.45));
}

static void test_retail_no_discount(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'R', 0.0, 0.0);

    ASSERT_DOUBLE_EQ(10000.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(0.00, q.disc_pct);
    ASSERT_DOUBLE_EQ(0.00, q.disc);
    ASSERT_DOUBLE_EQ(10000.00, q.net);
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(2030.00, q.vat);      /* (10000 + 150) * 0.20 */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(0.00, q.tradein);
    ASSERT_DOUBLE_EQ(12235.00, q.total);    /* 10000 + 150 + 2030 + 55 */
}

static void test_retail_salesman_discount(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'R', 5.0, 0.0);

    ASSERT_DOUBLE_EQ(10000.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(5.00, q.disc_pct);
    ASSERT_DOUBLE_EQ(500.00, q.disc);      /* 10000 * 0.05 */
    ASSERT_DOUBLE_EQ(9500.00, q.net);
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(1930.00, q.vat);      /* (9500 + 150) * 0.20 */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(11635.00, q.total);   /* 9500 + 150 + 1930 + 55 */
}

static void test_trade_automatic_10pct_discount(void)
{
    /* Locks in legacy behavior documented in KI-002: code applies 10%, not 5% */
    QUOTE q;
    calc_quote(&q, 10000.00, 'T', 0.0, 0.0);

    ASSERT_DOUBLE_EQ(10000.00, q.list);
    ASSERT_DOUBLE_EQ(1000.00, q.trade_disc); /* 10000 * 0.10 */
    ASSERT_DOUBLE_EQ(0.00, q.disc);
    ASSERT_DOUBLE_EQ(9000.00, q.net);
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(1830.00, q.vat);        /* (9000 + 150) * 0.20 */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(11035.00, q.total);     /* 9000 + 150 + 1830 + 55 */
}

static void test_trade_plus_salesman_discount(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'T', 10.0, 0.0);

    ASSERT_DOUBLE_EQ(10000.00, q.list);
    ASSERT_DOUBLE_EQ(1000.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(900.00, q.disc);        /* (10000 - 1000) * 0.10 */
    ASSERT_DOUBLE_EQ(8100.00, q.net);        /* 10000 - 1000 - 900 */
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(1650.00, q.vat);        /* (8100 + 150) * 0.20 */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(9955.00, q.total);      /* 8100 + 150 + 1650 + 55 */
}

static void test_salesman_discount_clamping(void)
{
    QUOTE q;

    /* Upper bound clamp: 15% max, documented in KI-004 */
    calc_quote(&q, 10000.00, 'R', 25.0, 0.0);
    ASSERT_DOUBLE_EQ(15.00, q.disc_pct);
    ASSERT_DOUBLE_EQ(1500.00, q.disc);
    ASSERT_DOUBLE_EQ(8500.00, q.net);

    /* Lower bound clamp: 0% min */
    calc_quote(&q, 10000.00, 'R', -10.0, 0.0);
    ASSERT_DOUBLE_EQ(0.00, q.disc_pct);
    ASSERT_DOUBLE_EQ(0.00, q.disc);
    ASSERT_DOUBLE_EQ(10000.00, q.net);
}

static void test_tradein_deduction(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'R', 0.0, 2000.00);

    ASSERT_DOUBLE_EQ(2000.00, q.tradein);
    ASSERT_DOUBLE_EQ(10235.00, q.total);     /* 12235 - 2000 */
}

static void test_reproduce_sample_invoice_50230(void)
{
    /* Invoice 50230: Vehicle 1009 (list 9295.00), Cust 2003 (R), 5% disc, 1500.00 tradein */
    QUOTE q;
    calc_quote(&q, 9295.00, 'R', 5.0, 1500.00);

    ASSERT_DOUBLE_EQ(9295.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(464.75, q.disc);
    ASSERT_DOUBLE_EQ(8830.25, q.net);
    ASSERT_DOUBLE_EQ(1796.05, q.vat);
    ASSERT_DOUBLE_EQ(1500.00, q.tradein);
    ASSERT_DOUBLE_EQ(9331.30, q.total);
}

static void test_reproduce_sample_invoice_50231(void)
{
    /* Invoice 50231: Vehicle 1012 (list 13495.00), Cust 2002 (T), 0% disc, 0.00 tradein */
    QUOTE q;
    calc_quote(&q, 13495.00, 'T', 0.0, 0.0);

    ASSERT_DOUBLE_EQ(13495.00, q.list);
    ASSERT_DOUBLE_EQ(1349.50, q.trade_disc);
    ASSERT_DOUBLE_EQ(0.00, q.disc);
    ASSERT_DOUBLE_EQ(12145.50, q.net);
    ASSERT_DOUBLE_EQ(2459.10, q.vat);
    ASSERT_DOUBLE_EQ(0.00, q.tradein);
    ASSERT_DOUBLE_EQ(14809.60, q.total);
}

static void test_reproduce_sample_invoice_50232(void)
{
    /* Invoice 50232: Vehicle 1001 (list 10495.00), Cust 2008 (Greenway Taxis, R), 8% disc, 0 tradein */
    QUOTE q;
    calc_quote(&q, 10495.00, 'R', 8.0, 0.0);

    ASSERT_DOUBLE_EQ(10495.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(839.60, q.disc);
    ASSERT_DOUBLE_EQ(9655.40, q.net);
    ASSERT_DOUBLE_EQ(1961.08, q.vat);
    ASSERT_DOUBLE_EQ(0.00, q.tradein);
    ASSERT_DOUBLE_EQ(11821.48, q.total);
}

/* ------------------------------------------------ TICKET-142 Fleet Tests */

static void test_fleet_customer_automatic_4pct_discount(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'F', 0.0, 0.0);

    ASSERT_DOUBLE_EQ(10000.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(400.00, q.fleet_disc);  /* 10000 * 0.04 */
    ASSERT_DOUBLE_EQ(0.00, q.disc);
    ASSERT_DOUBLE_EQ(9600.00, q.net);        /* 10000 - 400 */
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(1950.00, q.vat);        /* (9600 + 150) * 0.20 */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(0.00, q.tradein);
    ASSERT_DOUBLE_EQ(11755.00, q.total);     /* 9600 + 150 + 1950 + 55 */
}

static void test_fleet_customer_plus_salesman_discount(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'F', 5.0, 0.0);

    ASSERT_DOUBLE_EQ(10000.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(400.00, q.fleet_disc);  /* 10000 * 0.04 */
    ASSERT_DOUBLE_EQ(480.00, q.disc);        /* (10000 - 400) * 0.05 */
    ASSERT_DOUBLE_EQ(9120.00, q.net);        /* 10000 - 400 - 480 */
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(1854.00, q.vat);        /* (9120 + 150) * 0.20 */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(11179.00, q.total);     /* 9120 + 150 + 1854 + 55 */
}

static void test_fleet_customer_with_tradein(void)
{
    QUOTE q;
    calc_quote(&q, 10000.00, 'F', 0.0, 1500.00);

    ASSERT_DOUBLE_EQ(400.00, q.fleet_disc);
    ASSERT_DOUBLE_EQ(1500.00, q.tradein);
    ASSERT_DOUBLE_EQ(10255.00, q.total);     /* 11755 - 1500 */
}

static void test_greenway_taxis_quote_as_fleet(void)
{
    /* Greenway Taxis (ID 2008) purchasing vehicle 1001 (10495.00) as Fleet */
    QUOTE q;
    calc_quote(&q, 10495.00, 'F', 0.0, 0.0);

    ASSERT_DOUBLE_EQ(10495.00, q.list);
    ASSERT_DOUBLE_EQ(0.00, q.trade_disc);
    ASSERT_DOUBLE_EQ(419.80, q.fleet_disc);  /* rnd2(10495 * 0.04) */
    ASSERT_DOUBLE_EQ(0.00, q.disc);
    ASSERT_DOUBLE_EQ(10075.20, q.net);       /* 10495 - 419.80 */
    ASSERT_DOUBLE_EQ(150.00, q.delivery);
    ASSERT_DOUBLE_EQ(2045.04, q.vat);        /* rnd2((10075.20 + 150) * 0.20) */
    ASSERT_DOUBLE_EQ(55.00, q.reg_fee);
    ASSERT_DOUBLE_EQ(12325.24, q.total);     /* 10075.20 + 150 + 2045.04 + 55 */
}

int main(void)
{
    printf("Running Characterization & TDD Tests: Pricing Engine (calc.c)...\n\n");

    /* Regression Suite (Existing Behavior) */
    RUN_TEST(test_rnd2);
    RUN_TEST(test_retail_no_discount);
    RUN_TEST(test_retail_salesman_discount);
    RUN_TEST(test_trade_automatic_10pct_discount);
    RUN_TEST(test_trade_plus_salesman_discount);
    RUN_TEST(test_salesman_discount_clamping);
    RUN_TEST(test_tradein_deduction);
    RUN_TEST(test_reproduce_sample_invoice_50230);
    RUN_TEST(test_reproduce_sample_invoice_50231);
    RUN_TEST(test_reproduce_sample_invoice_50232);

    /* TDD Phase Suite: TICKET-142 (Expected to FAIL before implementation) */
    RUN_TEST(test_fleet_customer_automatic_4pct_discount);
    RUN_TEST(test_fleet_customer_plus_salesman_discount);
    RUN_TEST(test_fleet_customer_with_tradein);
    RUN_TEST(test_greenway_taxis_quote_as_fleet);

    TEST_REPORT();
}
