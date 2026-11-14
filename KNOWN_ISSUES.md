# KNOWN_ISSUES.md - Technical Debt and Issue Registry

This document records identified bugs, documentation discrepancies, legacy anomalies, or deferred architectural improvements.

---

## Registry Rules

1. **Deduplication**: Do not create duplicate entries for existing issues.
2. **Addendums for Related Findings**: If a new observation relates to an existing entry, append an addendum (`Addendum YYYY-MM-DD`) detailing the divergence or new context while keeping the parent issue unified.
3. **New Entries**: Only create a new identifier (`KI-XXX`) if the issue is conceptually distinct.
4. **Resolution Lifecycle**: When an issue is resolved or fixed:
   - Never delete the entry; keep it for historical traceability.
   - Update `Status` to `Resolved (<DATE> via <TASK_ID>)`.
   - Add a `Resolution:` field describing the resolution and changed files.
   - Move the entry from `## Active Issues` to `## Resolved Issues`.

---

## Active Issues

### [KI-002] Trade Customer Discount Discrepancy (Docs vs Implementation)
* **Status**: Accepted Legacy Behavior
* **Severity**: Important (Financial Logic)
* **Description**: `README.md` (line 66) and `main.c` help screen (line 500) state that Trade customers receive a 5% trade discount. However, `calc.c` (line 29) executes:
  ```c
  if (cust_type == 'T')
      q->trade_disc = rnd2(list * 0.10);
  ```
* **Impact**: Trade customers receive 10% discount in quotes and sales. Existing invoices in `data/invoices.dat` reflect 10% discount (e.g., invoice 50231 for vehicle 1012 with list 13495.00 has discount 1349.50 = 10%).
* **Resolution Strategy**: Preserve 10% in code to ensure 100% backward calculation compatibility. Update documentation in `README.md` to match reality.

---

### [KI-003] Delivery Fee Included in VAT Taxable Base
* **Status**: Accepted Legacy Behavior
* **Severity**: Important (Tax Logic)
* **Description**: `README.md` (lines 68-69) states that delivery (150.00) and registration (55.00) are VAT exempt. However, `calc.c` (line 40) calculates VAT as:
  ```c
  q->vat = rnd2((q->net + q->delivery) * 0.20);
  ```
* **Impact**: VAT 20% is levied on delivery (30.00 VAT on delivery fee). Existing invoices match this exact formula.
* **Resolution Strategy**: Maintain current implementation formula to prevent discrepancies in legacy invoice verification and test data.

---

### [KI-004] Salesman Discount PIN Missing and 15% Cap Discrepancy
* **Status**: Accepted Legacy Behavior
* **Severity**: Important (Security & Pricing)
* **Description**: `README.md` (line 67) and `main.c` (line 501) state that discounts above 10% require a manager PIN. However, `main.c` line 323 prompts for discount percentage without any PIN verification, and `calc.c` lines 33-35 clamp the discount at an arbitrary 15% ceiling (`if (disc_pct > 15) disc_pct = 15;`).
* **Impact**: Sales staff can enter discounts up to 15% without manager intervention. Discounts above 15% are silently capped at 15%.
* **Resolution Strategy**: Preserve existing calculation limits to prevent regressions. Documented for future access control hardening.

---

## Resolved Issues

### [KI-001] Missing report.c and Menu Option in README.md
* **Status**: Resolved (2026-11-13 via TICKET-142)
* **Severity**: Minor (Documentation)
* **Description**: Legacy `README.md` lines 29 and 57 referenced non-existent `report.c` and a menu item `6. Reports (monthly sales, margin per vehicle)`. Neither existed in `main.c`.
* **Resolution**: Completely revised `README.md`. Removed references to `report.c` and ghost menu option 6. Replaced outdated compilation instructions with standardized cross-platform build automation (`make` and `build.ps1`), accurate data directory locations (`./data/`), and correct pricing rules.
* **Changed Files**: `README.md`
