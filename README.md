# DealerDesk

Showroom management terminal application written in C for vehicle stock, customer records, price quotations, and invoices.

---

## Quick Start & Build System

The project provides standardized build and test automation for both POSIX (Linux, macOS, WSL) and Windows environments.

### Linux, macOS, WSL (GCC / Clang)

* **Build**:
  ```bash
  make
  ```
* **Run Tests**:
  ```bash
  make test
  ```
* **Clean**:
  ```bash
  make clean
  ```
* **Run Application**:
  ```bash
  ./dealerdesk
  ```

### Windows (PowerShell / MSVC)

The build script automatically detects Visual Studio Build Tools via `vswhere.exe`:

* **Build**:
  ```powershell
  .\build.ps1
  ```
* **Run Tests**:
  ```powershell
  .\build.ps1 -Test
  ```
* **Clean**:
  ```powershell
  .\build.ps1 -Clean
  ```
* **Run Application**:
  ```powershell
  .\dealerdesk.exe
  ```

---

## Storage & Data Files

Data files reside in `./data/` and use flat-file storage with pipe (`|`) field delimiters:

* `data/stock.dat`: Vehicle inventory, pricing, mileage, and status (0: Stock, 1: Reserved, 2: Sold).
* `data/customers.dat`: Customer records with ID, name, phone, email, and type (`R`: Retail, `T`: Trade, `F`: Fleet).
* `data/invoices.dat`: Sales invoices with list price, total discounts, net price, VAT, fees, trade-in deduction, and grand total.
* `data/next_ids.dat`: Sequence counters for vehicles, customers, and invoice numbering.

> [!IMPORTANT]
> The application rewrites entire `.dat` files upon mutations. To restore baseline fixtures:
> ```bash
> git checkout -- data/
> ```

---

## Application Menus

* **1. Stock**: List available stock, list all vehicles, add vehicle, find by plate, update price, reserve/release.
* **2. Customers**: List customers, add customer, find customer, edit customer (modify details or update customer type).
* **3. Quote**: Generate 7-day quotation with customer-specific pricing, salesman negotiation discounts, and trade-in deduction.
* **4. Sell vehicle**: Finalize sale, print invoice preview, confirm transaction, and save invoice record.
* **5. Invoices**: List existing invoices or inspect invoice details by number.
* **9. Help**: Show quick operational help.
* **0. Exit**: Exit application.

---

## Pricing Rules & Build-Up

Calculations are centralized in `calc.c`:

1. **Vehicle List Price**: Starting showroom vehicle price.
2. **Customer Discount**:
   * **Retail (`'R'`)**: 0% customer discount.
   * **Trade (`'T'`)**: 10% automatic trade discount on vehicle price.
   * **Fleet (`'F'`)**: 4% automatic fleet discount on vehicle price (TICKET-142).
3. **Salesman Discount**: Negotiated percentage (0% to 15%) applied to the vehicle price after customer discount (trade or fleet).
4. **Net Vehicle Price**: List price minus customer discount minus salesman discount.
5. **Delivery Fee**: £150.00 standard delivery fee (included in VAT taxable base).
6. **VAT (20%)**: 20% levied on `(net vehicle price + delivery fee)`.
7. **Registration Fee**: £55.00 government first registration fee (VAT exempt).
8. **Trade-in Deduction**: Subtracted directly from grand total.
9. **Total**: `net + delivery + VAT + registration fee - trade-in`.

---

## Continuous Integration

A multi-platform CI pipeline (`.github/workflows/ci.yml`) runs on push and pull requests targeting `master` across `ubuntu-latest` and `windows-latest`.
