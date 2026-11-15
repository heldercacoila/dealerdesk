/*
 * tests/test_smoke.c - Smoke tests for terminal menu navigation and screen output
 */
#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "test_harness.h"

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#define APP_CMD "dealerdesk.exe"
#else
#define POPEN popen
#define PCLOSE pclose
#define APP_CMD "./dealerdesk"
#endif

#define OUT_BUFFER_SIZE 16384

static int run_app_with_input(const char *input, char *output, size_t out_size)
{
    FILE *f_in;
    FILE *fp;
    size_t total_read = 0;
    char buffer[512];
    char cmd[512];

    output[0] = '\0';

    f_in = fopen("_smoke_in.tmp", "w");
    if (!f_in) {
        return -1;
    }
    fputs(input, f_in);
    fclose(f_in);

    snprintf(cmd, sizeof cmd, "%s < _smoke_in.tmp", APP_CMD);
    fp = POPEN(cmd, "r");
    if (!fp) {
        remove("_smoke_in.tmp");
        return -1;
    }

    while (fgets(buffer, sizeof buffer, fp)) {
        size_t len = strlen(buffer);
        if (total_read + len < out_size - 1) {
            strcat(output, buffer);
            total_read += len;
        }
    }

    PCLOSE(fp);
    remove("_smoke_in.tmp");
    return 0;
}

static void test_main_menu_options(void)
{
    char out[OUT_BUFFER_SIZE];
    int res = run_app_with_input("0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "DEALERDESK");
    ASSERT_STR_CONTAINS(out, "Main Menu");
    ASSERT_STR_CONTAINS(out, "1. Stock");
    ASSERT_STR_CONTAINS(out, "2. Customers");
    ASSERT_STR_CONTAINS(out, "3. Quote");
    ASSERT_STR_CONTAINS(out, "4. Sell vehicle");
    ASSERT_STR_CONTAINS(out, "5. Invoices");
    ASSERT_STR_CONTAINS(out, "9. Help");
    ASSERT_STR_CONTAINS(out, "0. Exit");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_stock_menu_options(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Select Stock (1), then Back (0), then Exit (0) */
    int res = run_app_with_input("1\n0\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "Stock");
    ASSERT_STR_CONTAINS(out, "1. List stock");
    ASSERT_STR_CONTAINS(out, "2. List all vehicles (incl. sold)");
    ASSERT_STR_CONTAINS(out, "3. Add vehicle");
    ASSERT_STR_CONTAINS(out, "4. Find by plate");
    ASSERT_STR_CONTAINS(out, "5. Change price");
    ASSERT_STR_CONTAINS(out, "6. Reserve / release vehicle");
    ASSERT_STR_CONTAINS(out, "0. Back");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_customers_menu_options(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Select Customers (2), then Back (0), then Exit (0) */
    int res = run_app_with_input("2\n0\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "Customers");
    ASSERT_STR_CONTAINS(out, "1. List customers");
    ASSERT_STR_CONTAINS(out, "2. Add customer");
    ASSERT_STR_CONTAINS(out, "3. Find customer");
    ASSERT_STR_CONTAINS(out, "4. Edit customer");
    ASSERT_STR_CONTAINS(out, "0. Back");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_invoices_menu_options(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Select Invoices (5), then Back (0), then Exit (0) */
    int res = run_app_with_input("5\n0\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "Invoices");
    ASSERT_STR_CONTAINS(out, "1. List invoices");
    ASSERT_STR_CONTAINS(out, "2. View invoice");
    ASSERT_STR_CONTAINS(out, "0. Back");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_help_screen_options(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Select Help (9), press Enter on pause, then Exit (0) */
    int res = run_app_with_input("9\n\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "Help");
    ASSERT_STR_CONTAINS(out, "DEALERDESK QUICK HELP");
    ASSERT_STR_CONTAINS(out, "Plates are converted to upper case automatically.");
    ASSERT_STR_CONTAINS(out, "Trade customers receive a 5% trade discount.");
    ASSERT_STR_CONTAINS(out, "Quotes are valid for 7 days.");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_invalid_option_handling(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Enter invalid option 99, press Enter on pause, then Exit (0) */
    int res = run_app_with_input("99\n\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "Invalid option");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_edit_customer_flow_prompt(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Select Customers (2), Edit (4), ID 9999 (not found), press Enter on pause, Back (0), Exit (0) */
    int res = run_app_with_input("2\n4\n9999\n\n0\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "Edit Customer");
    ASSERT_STR_CONTAINS(out, "Customer not found.");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void copy_file(const char *src_path, const char *dst_path)
{
    FILE *src = fopen(src_path, "rb");
    FILE *dst = fopen(dst_path, "wb");
    char buf[1024];
    size_t n;
    if (src && dst) {
        while ((n = fread(buf, 1, sizeof buf, src)) > 0)
            fwrite(buf, 1, n, dst);
    }
    if (src) fclose(src);
    if (dst) fclose(dst);
}

static void test_view_invoice_legacy(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Invoices (5), View invoice (2), Invoice 50230, press Enter, Back (0), Exit (0) */
    int res = run_app_with_input("5\n2\n50230\n\n0\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "INVOICE 50230");
    ASSERT_STR_CONTAINS(out, "List price");
    ASSERT_STR_CONTAINS(out, "Discounts");
    ASSERT_STR_CONTAINS(out, "TOTAL");
    ASSERT_STR_CONTAINS(out, "Goodbye.");
}

static void test_fleet_sale_and_view_invoice(void)
{
    char out[OUT_BUFFER_SIZE];
    /* Backup data files */
    copy_file("data/customers.dat", "data/customers.dat.bak");
    copy_file("data/stock.dat", "data/stock.dat.bak");
    copy_file("data/invoices.dat", "data/invoices.dat.bak");
    copy_file("data/next_ids.dat", "data/next_ids.dat.bak");

    /* 1. Edit customer 2008 to Fleet ('F'):
          Main Menu: Customers (2) -> Edit (4) -> ID (2008) -> keep name/phone/email (Enterx3) -> Type (F) -> pause (Enter) -> Back (0)
       2. Sell Vehicle 1002 to Customer 2008:
          Main Menu: Sell vehicle (4) -> Veh ID (1002) -> Cust ID (2008) -> Disc (0) -> Tradein (0) -> Confirm (Y) -> pause (Enter)
       3. View generated invoice 50233:
          Main Menu: Invoices (5) -> View invoice (2) -> ID (50233) -> pause (Enter) -> Back (0) -> Exit (0)
    */
    int res = run_app_with_input("2\n4\n2008\n\n\n\nF\n\n0\n4\n1002\n2008\n0\n0\nY\n\n5\n2\n50233\n\n0\n0\n", out, sizeof out);

    ASSERT_INT_EQ(0, res);
    ASSERT_STR_CONTAINS(out, "INVOICE 50233");
    ASSERT_STR_CONTAINS(out, "Fleet discount");
    ASSERT_STR_CONTAINS(out, "TOTAL");

    /* Restore pristine data files */
    copy_file("data/customers.dat.bak", "data/customers.dat");
    copy_file("data/stock.dat.bak", "data/stock.dat");
    copy_file("data/invoices.dat.bak", "data/invoices.dat");
    copy_file("data/next_ids.dat.bak", "data/next_ids.dat");
    remove("data/customers.dat.bak");
    remove("data/stock.dat.bak");
    remove("data/invoices.dat.bak");
    remove("data/next_ids.dat.bak");
}

int main(void)
{
    printf("Running Smoke Tests: Terminal Menu Navigation (test_smoke.c)...\n\n");

    RUN_TEST(test_main_menu_options);
    RUN_TEST(test_stock_menu_options);
    RUN_TEST(test_customers_menu_options);
    RUN_TEST(test_invoices_menu_options);
    RUN_TEST(test_help_screen_options);
    RUN_TEST(test_invalid_option_handling);
    RUN_TEST(test_edit_customer_flow_prompt);
    RUN_TEST(test_view_invoice_legacy);
    RUN_TEST(test_fleet_sale_and_view_invoice);

    TEST_REPORT();
}
