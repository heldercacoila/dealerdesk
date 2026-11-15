---
name: code-review-and-quality
description: Conducts multi-axis code review tailored for C applications and architecture. Use before finalizing any task or PR.
---

# Code Review and Quality (C & Systems Architecture)

## Overview

Multi-dimensional code review with quality gates. Every task must undergo review before closing or opening a pull request. Review covers five core axes: correctness, readability, architecture, security & safety, and performance.

Approval standard: Approve when the code improves overall software health, adheres to architectural guardrails, introduces zero regressions, and satisfies all task acceptance criteria.

---

## The Five-Axis Review for C Codebases

### 1. Correctness
- Does the code implement the exact specification without altering unrelated behaviors?
- Are boundary conditions handled (e.g., zero price, maximum discounts, empty strings)?
- Are calculation formulas consistent with legacy requirements (e.g. VAT, fee exemption, rounding via `rnd2`)?
- Does the change pass all tests (regression suite + new feature tests)?

### 2. Readability & Simplicity
- Are function and variable names descriptive, consistent, and concise?
- Is control flow simple and idiomatic C without unnecessary nested branches?
- Are structures and headers properly scoped with include guards (`#ifndef HEADER_H`)?
- Are comments used only where intent or business logic is non-obvious?

### 3. Architecture & Modularity
- Does the change respect layer boundaries:
  - Financial calculations strictly in `calc.c`
  - Persistence and disk I/O strictly in `dbfile.c`
  - Screen rendering and formatting in `screen.c` and `main.c`
- Is state encapsulated properly without adding uncontrolled global variables?
- Are data structures extensible without breaking backwards binary/text layouts?

### 4. Security & Safety (C Specific)
- **Buffer Safety**: Are all string operations bounded (`fgets` with explicit size, `strncpy` with forced null-termination)?
- **Delimiter Sanitization**: Are user inputs checked and sanitized against delimiter corruption (`|` converted to `/`)?
- **Pointer Safety**: Are pointers validated against `NULL` before dereferencing?
- **Resource Management**: Are all opened file handles (`fopen`) safely closed (`fclose`) on all execution paths?
- **Compilation Cleanliness**: Does the code compile with zero warnings under:
  - GCC/Clang: `-Wall -Wextra -pedantic`
  - MSVC: `/W3 /D_CRT_SECURE_NO_WARNINGS`

### 5. Performance & Resource Limits
- Are static arrays protected against capacity overflow (`MAX_VEH`, `MAX_CUST`, `MAX_INV`)?
- Are search loops bounded and linear without unnecessary redundant traversals?
- Is memory overhead negligible and statically bounded?

---

## Severity Classification

| Severity | Definition | Required Action |
|---|---|---|
| **Critical** | Safety vulnerability (buffer overflow, memory corruption), data corruption, broken calculation, regression in existing features. | **Blocks task completion.** Must be resolved before proceeding. |
| **Important** | Sub-optimal architectural coupling, missing unit test coverage for edge case, missing input bounds. | Address if in scope; if out of scope, log in `KNOWN_ISSUES.md`. |
| **Minor / Nit** | Formatting consistency, naming preference, minor comment clarity. | Optional; author discretion or document in `KNOWN_ISSUES.md`. |

---

## Review Checklist

```markdown
## Code Review Summary

### Context & Scope
- Task ID / Title:
- Files Changed:

### Findings
- [ ] Correctness: Verified against acceptance criteria and regression tests.
- [ ] Readability: Clean, idiomatic C with clear naming.
- [ ] Architecture: Proper layer separation (calc vs dbfile vs screen).
- [ ] Security & C Safety: Bounded buffers, null checks, delimiter sanitization, zero warnings.
- [ ] Performance: Static bounds protected, no resource leaks.

### Verdict
- [ ] Approved (All critical checks pass; non-critical items logged in KNOWN_ISSUES.md).
- [ ] Changes Requested (Critical issues identified).
```
