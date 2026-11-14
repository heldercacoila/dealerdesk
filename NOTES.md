# Engineering Reflection: DealerDesk Modernization & TICKET-142

## 1. Summary of Changes & Prioritization Rationale

When approaching a legacy codebase with zero automated tests, manual releases, and conflicting documentation, our primary objective was establishing safety and repeatability before touching business logic. The work was phased in strict order of dependency:

1. **Governance & Agent Setup (`AGENTS.md`, `.agent/skills/`)**: Defined engineering contracts, safety guardrails (bounded buffers, pipe delimiter sanitization, compiler cleanliness), and the 5-stage lifecycle.
2. **Build Automation & In-Tree Testing (`Makefile`, `build.ps1`, `tests/test_harness.h`)**: Created a dual-platform build system for Linux/macOS (GCC/Clang) and Windows (MSVC via `vswhere.exe`). Implemented an in-tree pure C assertion framework with zero external dependencies, establishing 14 characterization tests that locked down pricing calculations and flat-file parsing to the penny against historical production invoices (50230, 50231, 50232).
3. **Continuous Integration (`.github/workflows/ci.yml`)**: Automated multi-OS builds and test execution on `ubuntu-latest` and `windows-latest` for pushes and pull requests, establishing a cloud quality gate.
4. **TDD Specification for TICKET-142 (`tests/test_calc.c`)**: Introduced unit tests defining the 4% fleet discount and its interaction with salesman discounts, verifying a failing (red) state prior to code modification.
5. **Terminal Navigation Smoke Testing (`tests/test_smoke.c`)**: Before modifying `main.c`, created automated smoke tests executing the application binary via standard input redirection to ensure existing menu options were characterized and protected against regression.
6. **Feature Implementation (TICKET-142)**: Implemented 4% fleet discount in `calc.c`, mapped customer type `'F'` in `globals.h` and `main.c`, added customer editing (`4. Edit customer` in customer menu) to allow updating existing accounts like Greenway Taxis, added a dedicated fleet discount line to quotations, and consolidated discounts in invoices without breaking flat-file schemas.

---

## 2. AI Agent Pairing Experience & Error Recovery

Working with AI coding agents accelerated repetitive tasks, such as generating cross-platform build scripts, characterization assertions from invoice samples, and dual-compiler flags. However, rigorous human oversight was essential to enforce discipline and catch procedural errors.

### Critical Procedural Breach & Human Gatekeeper Rule
Early in Phase 1 (commit `17286e9`), the agent autonomously executed a `git commit` and `git push` without awaiting explicit human confirmation. The human engineer immediately caught this unauthorized action and instituted a strict corrective constraint:
* Codified the **Strict Human Gatekeeper Rule** into `AGENTS.md` and created modular git skills (`.agent/skills/git-commit/` and `.agent/skills/git-push/`).
* The agent is strictly prohibited from running git mutations autonomously. It must summarize the diff, propose a conventional commit message, and **STOP AND WAIT** for explicit authorization (`commit`, `push`, `proceed`).
* This rule prevented unintended code leaks and maintained clean, auditable atomic commit boundaries throughout the project.

### Automated Test Interception of Menu Assumptions
When authoring the terminal menu smoke tests, an initial assumption assumed the vehicle stock menu listed `2. Add vehicle`. The smoke test failed immediately on execution: the legacy application actually had `2. List all vehicles (incl. sold)` and `3. Add vehicle`. The characterization test caught the discrepancy before any UI alterations were made, demonstrating the necessity of locking down legacy truth before refactoring.

---

## 3. Assumptions Log

* **Trade Discount (10% vs 5%)**: `README.md` claimed 5%, but `calc.c` applied 10%, and sample invoice 50231 proved that 10% was billed in production. We retained 10% in code to prevent calculation regressions, cataloging the discrepancy in `KNOWN_ISSUES.md` [KI-002] and updating `README.md`.
* **Delivery Fee VAT Inclusion**: `README.md` stated delivery is VAT-exempt, but `calc.c` includes delivery in the 20% VAT base (`rnd2((q->net + q->delivery) * 0.20)`), matching production invoices. We preserved the formula to maintain 100% backward compatibility (cataloged as [KI-003]).
* **Salesman Discount Ceiling**: `README.md` mentioned a manager PIN for discounts over 10%, but `main.c` prompted for discount with no PIN logic, and `calc.c` clamped discounts at 15%. We maintained the 15% clamping in code and documented the missing access control in [KI-004].
* **Flat-File Storage Invariance**: Stored fleet discounts combined into the existing `disc` column in `invoices.dat`. This satisfied the requirement for quotes to show a separate line item while ensuring existing and new invoices load seamlessly without data migrations.

---

## 4. Future Backlog (With More Time)

1. **Input Validation & Dynamic Capacity**: Replace static bounds (`MAX_VEH`, `MAX_CUST`, `MAX_INV`) with dynamic allocation or robust capacity checks with explicit error dialogs.
2. **Access Control & Manager PIN**: Implement the missing manager PIN prompt for salesman discounts exceeding 10% to resolve [KI-004].
3. **Relational Database Migration**: Transition from flat pipe-delimited files (`./data/*.dat`) to SQLite to provide atomic transactions, concurrency safety, and query capability.
4. **End-to-End Automated Workflows**: Expand `tests/test_smoke.c` to run complete multi-step transaction lifecycles (add vehicle -> reserve -> quote -> sell -> verify invoice) in sandboxed temporary data directories.
