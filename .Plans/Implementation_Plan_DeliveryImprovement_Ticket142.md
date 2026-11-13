# Implementation Plan: Delivery Pipeline Modernization and TICKET-142 (Fleet Discount)

## 1. Context and Objectives

DealerDesk is an operational C terminal application used for car dealership management (stock, customers, quotes, invoices). The project lacks automated testing, continuous integration, standardized multi-platform build scripts, and has documented discrepancies with existing code.

This plan executes the two-phase assessment on branch `TICKET-142-Fleet_discount_on_quotes_and_invoices`:
1. **Improve Delivery Pipeline & Governance**:
   - Establish governance contracts and agent guidelines (`AGENTS.md`).
   - Project-level skills in `.agent/skills/` (`code-review-and-quality`, `git-commit`, `git-push`, `create-pr`).
   - Build automation (`Makefile` for GCC/Clang on Linux/macOS/WSL, `build.ps1` for MSVC on Windows).
   - Automated testing harness in pure C (`tests/test_harness.h`, `tests/test_calc.c`, `tests/test_dbfile.c`).
   - Continuous Integration with GitHub Actions (`.github/workflows/ci.yml`).
   - Technical issue tracking via `KNOWN_ISSUES.md`.
2. **Implement TICKET-142 (Fleet Discount)**:
   - Introduce customer type `'F'` (Fleet).
   - Allow setting customer type when adding customers or editing existing customers (e.g. Greenway Taxis, ID 2008).
   - Apply automated 4% fleet discount on vehicle list price with a dedicated display line in quotes and invoices.
   - Maintain 100% backwards compatibility with existing `.dat` files and non-fleet calculations.

---

## 2. Technical Decisions & Alignment

* **Plan Location**: Stored under `.Plans/`.
* **Branching Strategy**: Work executed on task branch `TICKET-142-Fleet_discount_on_quotes_and_invoices`.
* **Testing & TDD Lifecycle**:
  - Regression suite for existing features must remain 100% green at all times.
  - New test cases for TICKET-142 will be committed in the initial TDD phase in a failing (red) state.
  - Implementation must be architecturally solid, modular, and safe (not merely minimal code to pass tests).
* **Skills Separation**:
  - `git-commit`: Dedicated commit skill using Conventional Commits; triggerable via command or natural language.
  - `git-push`: Dedicated push skill; triggerable via command or natural language, independently or in sequence.
  - `code-review-and-quality`: In-tree code review skill adapted for C (memory safety, buffer bounds, pointer validation, zero warnings).
  - `create-pr`: Dedicated Draft PR creation skill, strictly restricted to invocation via `/Create-pr`.
* **Known Issues Tracking (`KNOWN_ISSUES.md`)**:
  - Review non-critical findings or deferred scope.
  - Check existing entries: do not duplicate identical issues; add addendums to related existing issues explaining divergences; create new entries only for distinct issues.

---

## 3. External Dependencies Evaluation

| Tool / Component | Type | License | Cost | Status |
|---|---|---|---|---|
| **Custom C Test Harness (`tests/test_harness.h`)** | In-tree header | Free / Open | Totally free ($0) | In-tree; zero runtime or package dependencies |
| **Build Scripts (`Makefile`, `build.ps1`)** | Native shell / PowerShell | Free / Open | Totally free ($0) | In-tree |
| **GitHub Actions** | CI platform | Free tier | Totally free ($0) | Standard workflow |
| **GitHub CLI (`gh`)** | CLI tool | MIT | Totally free ($0) | Pre-installed locally and on CI runners |

*Zero commercial or paid dependencies are introduced.*

---

## 4. Acceptance Criteria

1. **Governance & Tooling**:
   - `AGENTS.md` establishes the 5-stage lifecycle, project invariants, C safety standards, and commit/PR workflows.
   - Project-level skills installed in `.agent/skills/`.
   - `Makefile` compiles clean binaries on Linux/macOS with GCC/Clang (`-Wall -Wextra -pedantic`).
   - `build.ps1` compiles clean binaries on Windows with MSVC (`/W3 /D_CRT_SECURE_NO_WARNINGS`).
   - CI workflow executes multi-platform build and test matrix on Ubuntu and Windows.
2. **Regression & Safety**:
   - Characterization tests verify legacy pricing calculations for Retail and Trade customers to the penny.
   - Serialization and parsing of pipe-delimited records in `.dat` files verified without data corruption.
3. **TICKET-142 Features**:
   - Customer type `'F'` selectable during customer creation and available for modifying existing customers.
   - Greenway Taxis (ID 2008) can be modified to Fleet type.
   - Quote for Fleet customer applies 4% discount on vehicle list price and displays `Fleet discount` line.
   - Salesman discount applies on vehicle price after fleet discount; fees and VAT calculated correctly.
   - Existing non-fleet quotes and invoices unchanged to the penny.
   - Invoices record total discount correctly; existing invoices in `invoices.dat` display correctly.
4. **Code Quality & Review**:
   - `/code-review-and-quality` executed prior to task completion with zero critical findings.
   - Non-critical findings cataloged in `KNOWN_ISSUES.md`.
   - `/Create-pr` publishes a Draft PR with structured summary.

---

## 5. Phased Task Breakdown

### Phase 1: Governance, Skills & Project Context
- [x] Task 1.1: Author [AGENTS.md](AGENTS.md) with the 5-stage lifecycle, C safety guardrails, data invariants, and command contracts.
- [x] Task 1.2: Install and adapt [`.agent/skills/code-review-and-quality/SKILL.md`](.agent/skills/code-review-and-quality/SKILL.md) for C projects.
- [x] Task 1.3: Install [`.agent/skills/git-commit/SKILL.md`](.agent/skills/git-commit/SKILL.md) and [`.agent/skills/git-push/SKILL.md`](.agent/skills/git-push/SKILL.md).
- [x] Task 1.4: Install [`.agent/skills/create-pr/SKILL.md`](.agent/skills/create-pr/SKILL.md) with strict `/Create-pr` trigger constraint.
- [x] Task 1.5: Initialize [KNOWN_ISSUES.md](KNOWN_ISSUES.md) cataloging initial legacy discrepancies (e.g., missing `report.c`, 10% vs 5% trade discount docs discrepancy).
- [x] Task 1.6: Atomic commit for Phase 1 governance files.

### Phase 2: Build Automation & Characterization Test Harness
- [x] Task 2.1: Implement in-tree C test harness in `tests/test_harness.h`.
- [x] Task 2.2: Implement pricing characterization tests in `tests/test_calc.c` (locking down Retail, Trade, and legacy VAT/fee behaviors).
- [x] Task 2.3: Implement persistence characterization tests in `tests/test_dbfile.c`.
- [x] Task 2.4: Create cross-platform `Makefile` and `build.ps1` with build and test targets.
- [x] Task 2.5: Configure GitHub Actions CI workflow in `.github/workflows/ci.yml`.
- [x] Task 2.6: Execute full regression suite (must be 100% green).
- [x] Task 2.7: Atomic commit for Phase 2 infrastructure and regression tests.

### Phase 3: TICKET-142 - TDD Specification (Failing Tests)
- [x] Task 3.1: Add unit tests in `tests/test_calc.c` for 4% Fleet discount calculation, interaction with salesman discount, and quote formatting.
- [x] Task 3.2: Verify characterization/regression tests remain 100% green while new Fleet tests fail (red state).
- [x] Task 3.3: Atomic commit for Phase 3 failing tests (`test: add failing tests for ticket-142 fleet discount`).

### Phase 4: TICKET-142 - Robust Implementation & Architecture
- [x] Task 4.1: Implement Terminal Menu Smoke Test suite (verifying Main, Stock, Customers, and Invoices menus integrity).
- [x] Task 4.2: Update `QUOTE` struct in `calc.h` and implement fleet calculation in `calc.c`.
- [x] Task 4.3: Update customer model in `globals.h` and `main.c` (support `'F'`, update prompt in `add_customer()`, implement customer editing `edit_customer()`).
- [x] Task 4.4: Update quote display in `main.c` to render the separate "Fleet discount" line.
- [x] Task 4.5: Update `sell_vehicle()` in `main.c` to consolidate fleet discount into invoice totals.
- [x] Task 4.6: Run test suite to verify all tests (regression + new fleet tests + menu smoke tests) are 100% green.
- [ ] Task 4.7: Atomic commit for Phase 4 implementation.

### Phase 5: Verification, Code Review, Documentation & Draft PR
- [ ] Task 5.1: Execute end-to-end manual terminal verification (add fleet customer, edit Greenway Taxis, generate quote, process sale).
- [ ] Task 5.2: Execute full test suite on MSVC and GCC.
- [ ] Task 5.3: Run `/code-review-and-quality` review; address critical issues; update `KNOWN_ISSUES.md` with non-critical observations.
- [ ] Task 5.4: Review and update documentation: fix build instructions in `README.md`, update `AGENTS.md` with learned lessons, author `NOTES.md` reflecting on AI agent pairing.
- [ ] Task 5.5: Final atomic commit for documentation and review fixes.
- [ ] Task 5.6: Invoke `/Create-pr` to publish Draft Pull Request.

---

## 6. Execution Decisions & Alignment Log

This log registers decisions, clarifications, and user directions made throughout execution:

* **2026-11-11 - Plan Storage**: Plans are stored in `.Plans/` in the project.
* **2026-11-11 - In-Tree Code Review Skill**: Adapted `/code-review-and-quality` into `.agent/skills/` tailored to C memory/pointer/buffer safety.
* **2026-11-11 - Modular Git Skills**: Split commit and push into two separate skills (`git-commit` and `git-push`), invokable via command or natural language, independently or in sequence.
* **2026-11-11 - Strict PR Invocations**: Guardrail implemented in `create-pr` requiring explicit `/Create-pr` command; conversational requests are rejected.
* **2026-11-11 - TDD Commit Rule**: Initial testing phase permits new feature tests to be in a failing (red) state during commit, provided regression tests are 100% green.
* **2026-11-11 - Zero Autonomous Commits/Pushes**: Hard human gatekeeper rule codified. The agent is strictly prohibited from executing `git commit` or `git push` autonomously. It must propose the action, diff summary, and commit message, and await explicit authorization before running git mutation commands.
* **2026-11-12 - Characterization Tests & KI-004**: Implemented in-tree test harness and 14 characterization tests for pricing and storage. Identified and cataloged KI-004 documenting missing salesman discount PIN and real 15% discount limit.
* **2026-11-12 - Multi-Platform Build Automation & Dual Validation**: Created `Makefile` (GCC/Clang) and `build.ps1` (MSVC) with automatic environment detection via `vswhere.exe`, test execution, and clean targets. Dual-platform execution verified locally with 100% passing tests and zero warnings on both Windows (MSVC) and Linux (WSL Ubuntu with GCC `-Wall -Wextra -pedantic`).
* **2026-11-12 - Continuous Integration (GitHub Actions)**: Created multi-platform CI workflow (`.github/workflows/ci.yml`) targeting Ubuntu and Windows runners on push/PR, including `workflow_dispatch` for manual on-demand execution. Verified 100% green on GitHub Actions cloud runners.
* **2026-11-12 - TDD Red State Established**: Introduced 4 failing unit tests in `tests/test_calc.c` defining 4% fleet discount behaviors. Verified that all 10 existing regression tests pass while the 4 new fleet tests fail with `Expected 400.00, got 0.00`.
* **2026-11-13 - Terminal Menu Smoke Tests (Missing Test Suite Decision)**: In accordance with AGENTS.md Stage 3 governance, the user accepted Option A to implement automated smoke tests for terminal menu navigation (verifying Main, Stock, Customers, Invoices, and Help options).
* **2026-11-13 - Terminal Menu Smoke Tests Executed**: Implemented in-tree smoke test runner `tests/test_smoke.c` and extended `tests/test_harness.h` with `ASSERT_STR_CONTAINS`. Validated 6 smoke test scenarios covering Main, Stock, Customers, Invoices, Help, and invalid option handling across Windows (MSVC) and Linux (WSL GCC) with 100% green results. Integrated into `Makefile` and `build.ps1`.
* **2026-11-13 - Phase 4 Implementation & Dual-Platform Verification**: Implemented 4% fleet discount for customer type 'F' in `calc.c`, with salesman discount applied to post-fleet vehicle price. Extended customer model in `globals.h` and `main.c` with 'F' display, updated `add_customer()` prompt, added `edit_customer()` (option 4 in customer menu) to allow editing existing customers like Greenway Taxis, added dedicated Fleet discount line in `print_quote()`, and consolidated total discount in `sell_vehicle()` preserving `invoices.dat` schema compatibility. Verified all 25 automated tests 100% green on Windows (MSVC) and Linux (WSL GCC).
* **2026-11-11 - Plan Living Status Updates**: Implementation plan must track status dynamically with `[x]` upon completion of each task and record all design/acceptance decisions in this section.
