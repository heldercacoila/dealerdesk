/*
 * tests/test_dbfile.c - Characterization tests for flat-file persistence layer
 */
#include "test_harness.h"
#include "../globals.h"
#include "../dbfile.h"

/* Define global tables required by dbfile.c (normally defined in main.c) */
VEHICLE  veh[MAX_VEH];
int      nveh = 0;
CUSTOMER cust[MAX_CUST];
int      ncust = 0;
INVOICE  inv[MAX_INV];
int      ninv = 0;

int next_veh_id  = 0;
int next_cust_id = 0;
int next_inv_no  = 0;

static void test_load_all_counts(void)
{
    load_all();

    ASSERT_INT_EQ(12, nveh);
    ASSERT_INT_EQ(8, ncust);
    ASSERT_INT_EQ(3, ninv);

    ASSERT_INT_EQ(1013, next_veh_id);
    ASSERT_INT_EQ(2009, next_cust_id);
    ASSERT_INT_EQ(50233, next_inv_no);
}

static void test_vehicle_parsing(void)
{
    load_all();

    /* First vehicle: 1001|LK19 XRT|Ford|Fiesta 1.0 EcoBoost Zetec|2019|38210|7900.00|10495.00|2 */
    ASSERT_INT_EQ(1001, veh[0].id);
    ASSERT_STR_EQ("LK19 XRT", veh[0].plate);
    ASSERT_STR_EQ("Ford", veh[0].make);
    ASSERT_STR_EQ("Fiesta 1.0 EcoBoost Zetec", veh[0].model);
    ASSERT_INT_EQ(2019, veh[0].year);
    ASSERT_LONG_EQ(38210, veh[0].mileage);
    ASSERT_DOUBLE_EQ(7900.00, veh[0].cost);
    ASSERT_DOUBLE_EQ(10495.00, veh[0].price);
    ASSERT_INT_EQ(ST_SOLD, veh[0].status);

    /* Second vehicle: in stock */
    ASSERT_INT_EQ(1002, veh[1].id);
    ASSERT_INT_EQ(ST_STOCK, veh[1].status);
}

static void test_customer_parsing(void)
{
    load_all();

    /* 2001|Sarah Whitfield|07700 900412|s.whitfield@example.com|R */
    ASSERT_INT_EQ(2001, cust[0].id);
    ASSERT_STR_EQ("Sarah Whitfield", cust[0].name);
    ASSERT_STR_EQ("07700 900412", cust[0].phone);
    ASSERT_STR_EQ("s.whitfield@example.com", cust[0].email);
    ASSERT_TRUE(cust[0].type == 'R');

    /* 2002|Marlow Motors Ltd|...|T */
    ASSERT_INT_EQ(2002, cust[1].id);
    ASSERT_TRUE(cust[1].type == 'T');

    /* 2008|Greenway Taxis|...|R */
    ASSERT_INT_EQ(2008, cust[7].id);
    ASSERT_STR_EQ("Greenway Taxis", cust[7].name);
    ASSERT_TRUE(cust[7].type == 'R');
}

static void test_invoice_parsing(void)
{
    load_all();

    /* 50230|2021-02-11|1009|2003|9295.00|464.75|8830.25|1796.05|205.00|1500.00|9331.30 */
    ASSERT_INT_EQ(50230, inv[0].no);
    ASSERT_STR_EQ("2021-02-11", inv[0].date);
    ASSERT_INT_EQ(1009, inv[0].veh_id);
    ASSERT_INT_EQ(2003, inv[0].cust_id);
    ASSERT_DOUBLE_EQ(9295.00, inv[0].list);
    ASSERT_DOUBLE_EQ(464.75, inv[0].disc);
    ASSERT_DOUBLE_EQ(8830.25, inv[0].net);
    ASSERT_DOUBLE_EQ(1796.05, inv[0].vat);
    ASSERT_DOUBLE_EQ(205.00, inv[0].fees);
    ASSERT_DOUBLE_EQ(1500.00, inv[0].tradein);
    ASSERT_DOUBLE_EQ(9331.30, inv[0].total);
}

int main(void)
{
    printf("Running Characterization Tests: Flat-file Storage (dbfile.c)...\n\n");

    RUN_TEST(test_load_all_counts);
    RUN_TEST(test_vehicle_parsing);
    RUN_TEST(test_customer_parsing);
    RUN_TEST(test_invoice_parsing);

    TEST_REPORT();
}
