# AGENTS.md - Engineering Governance and Agent Guidelines

## 1. System Overview

DealerDesk is an operational, menu-driven terminal application written in C (C89/C99 standard). It manages dealership vehicle stock, customer records, pricing quotations, and invoices.

The application uses flat-file storage in `./data/` delimited by pipe (`|`) characters. Data is loaded into static in-memory structures and rewritten in full upon mutations.

---

## 2. Five-Stage Development Lifecycle

Every engineering task or feature ticket must follow this strict 5-stage lifecycle.

```
[Stage 1: Clarification & Alignment] 
                 │
                 ▼
[Stage 2: Plan (.Plans/) & Task Branch]
                 │
                 ▼
[Stage 3: Implementation & Tests] ──(per phase)──> [Stage 4: Atomic Commit]
                 │                                 (TDD Phase: New=Red, Regr=Green)
                 ▼ (all phases complete)
[Code Review (/code-review-and-quality) & KNOWN_ISSUES.md]
                 │
                 ▼
[Stage 5: Documentation Review & Draft PR via /Create-pr]
```

### Stage 1: Clarification & Alignment
* **Input**: Task definition or ticket (e.g., `TICKET-142.md`).
* **Required Agent Actions**:
  1. Present technical interpretation of the task within the existing codebase.
  2. Question the user regarding any omissions, ambiguities, legacy discrepancies, or boundary conditions.
* **Exit Gate**: No code or final implementation plan may proceed without explicit user confirmation of alignment decisions.

### Stage 2: Phased Implementation Plan & Task Branching
* **Storage Location**: Implementation plans must be saved under `.Plans/Implementation_Plan_<ID_OR_NAME>.md`.
* **Required Plan Sections**:
  1. General description and technical objectives.
  2. Design choices and Q&A alignments from Stage 1.
  3. Verifiable acceptance criteria.
  4. Phased implementation breakdown:
     - **Regression Phase**: Characterization tests for existing code to be touched (must be 100% green).
     - **TDD Phase**: New tests defining the expected behavior (failing/red before code exists).
     - **Robust Implementation Phase**: Solid, modular, and safe implementation (not merely minimal code to pass tests).
  5. **Living Document Requirement**:
     - The plan must be updated dynamically as implementation proceeds, ticking completed tasks (`[x]`).
     - An **Execution Decisions & Alignment Log** section must record every design clarification, user resolution of ambiguities, or scoped trade-offs decided during execution.
* **Branch Creation**: Prior to starting implementation, create and switch to a task branch named `<TASK_ID>-<Task_Title_With_Underscores>` (e.g., `TICKET-142-Fleet_discount_on_quotes_and_invoices`).
* **Exit Gate**: The user must explicitly approve the plan before file changes begin.

### Stage 3: Implementation, Test Governance & Guardrails
* **Test Coverage Rule**: Every new testable function or logic branch must be accompanied by automated tests.
* **Missing Test Suites**: If a required category of testing is absent (e.g., terminal E2E tests):
  - The agent must formally suggest its creation.
  - The user must explicitly decide (Accept or Decline).
  - **Blocking Gate**: The task cannot be marked done without an explicit decision from the user recorded.
* **Execution Scope**:
  - *Intermediate phases*: Run new tests and existing tests exercising modified paths.
  - *Final phase*: Execute all project tests (100% passing) and run `/code-review-and-quality`.
* **Review Findings**:
  - *Critical*: Blocks completion until resolved.
  - *Non-critical / Out-of-scope*: Logged into `KNOWN_ISSUES.md` (following the deduplication/addendum policy) after discussing with the user.

### Stage 4: Atomic Commits & Git Cadence
* **Cadence**: Suggest a commit immediately upon completing each plan phase.
* **TDD Phase Commit Rule**: During the initial TDD test phase, existing regression tests must be green, but newly added tests are permitted (and expected) to fail (`test: add failing tests for...`).
* **Skills**:
  - `git-commit`: Analyzes diff, generates Conventional Commit message, requests user confirmation, executes commit.
  - `git-push`: Pushes branch commits to remote on demand.
  - Both skills can be triggered via natural language or command, individually or in sequence.

### Stage 5: Documentation Review, Guardrails & Draft PR
* **Documentation Review**:
  - Update `README.md` if build commands, execution options, or operational rules change.
  - Update `AGENTS.md` if new technical constraints, pitfalls, or best practices are uncovered.
  - Update `KNOWN_ISSUES.md` with any deferred technical observations.
* **Draft Pull Request**:
  - Suggest opening a PR targeting `master`.
  - Execute via the dedicated skill `create-pr`.
  - **Strict Constraint**: The PR skill can **only** be triggered via `/Create-pr`. Plain text requests will be rejected with an instruction to use `/Create-pr`.

---

## 3. C Safety & Architectural Guardrails

### 3.1 Memory, Buffers and Pointers
* **Buffer Overflow Prevention**: Use bounded reading functions (`fgets`, `strncpy` with null terminator guarantee). Never use unbounded `gets` or `strcpy`.
* **Static Limits**: Respect static bounds in `globals.h` (`MAX_VEH`, `MAX_CUST`, `MAX_INV`). Always guard array appends against buffer capacity.
* **Pointer Safety**: Verify pointers before dereferencing, especially return values from `find_veh`, `find_cust`, and `fopen`.
* **Compiler Cleanliness**: Code must compile without warnings under:
  - GCC/Clang: `-Wall -Wextra -pedantic`
  - MSVC: `/W3 /D_CRT_SECURE_NO_WARNINGS`

### 3.2 Data Layer Invariants
* **Delimiter Sanitization**: The pipe symbol (`|`) is the field separator in `.dat` files. Any user input containing `|` must be replaced with `/` to avoid schema corruption.
* **Backward Compatibility**: Existing files (`stock.dat`, `customers.dat`, `invoices.dat`, `next_ids.dat`) must load without manual schema conversion.
* **Numeric Rounding**: All currency calculations must be rounded to two decimal places using `rnd2()` in `calc.c`.
* **Separation of Concerns**:
  - Pricing logic lives in `calc.c`.
  - File I/O lives in `dbfile.c`.
  - UI presentation and menus live in `screen.c` and `main.c`.

---

## 4. Known Issues Policy (`KNOWN_ISSUES.md`)

When non-critical issues, legacy anomalies, or out-of-scope improvements are identified:
1. **Search Existing Entries**: Check if the issue has already been logged.
2. **Avoid Duplication**: If the exact issue exists, do not duplicate it.
3. **Add Addendum**: If the finding is related to an existing issue, append an addendum to the existing entry detailing the specific divergence or context while preserving the relationship.
4. **Create New Entry**: Only create a new numbered item if the issue is conceptually distinct.
5. **Resolution Lifecycle**: When an issue is resolved or fixed:
   - Never delete the entry; preserve historical traceability.
   - Update `Status` to `Resolved (<DATE> via <TASK_ID>)`.
   - Add a `Resolution:` field explaining the fix and modified files.
   - Move the entry from `## Active Issues` to `## Resolved Issues`.

---

## 5. Build and Test Command Contracts

* **Linux / macOS / WSL**:
  - Build: `make`
  - Test: `make test`
  - Clean: `make clean`
  - Run: `./dealerdesk`
* **Windows (PowerShell)**:
  - Build: `.\build.ps1`
  - Test: `.\build.ps1 -Test`
  - Clean: `.\build.ps1 -Clean`
  - Run: `.\dealerdesk.exe`
