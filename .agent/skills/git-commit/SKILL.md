---
name: git-commit
description: Analyzes git status and diff, creates a standardized Conventional Commit, and requests user confirmation before committing.
---

# Git Commit Skill

## Critical Constraint

> [!CAUTION]
> **Zero Autonomous Commits**: The agent is **strictly forbidden** from running `git commit` without explicit, unambiguous user authorization. Propose the diff summary and commit message, then stop and wait for user approval.

---

## Purpose

Standardize atomic git commits following the Conventional Commits specification. Can be invoked via natural language (e.g., "commit phase 1", "commit current changes") or direct command.

---

## Operating Procedure

1. **Inspect Status & Diff**:
   - Run `git status` to see modified and untracked files.
   - Run `git diff` to understand exact code changes.
2. **Classify Conventional Commit Type**:
   - `feat:` new features or business capability.
   - `test:` adding or updating tests (including failing TDD tests).
   - `refactor:` code restructuring without behavioral change.
   - `fix:` bug fixes or calculation corrections.
   - `docs:` documentation, plans, or guidelines (`README.md`, `AGENTS.md`, `.Plans/`).
   - `chore:` build scripts, CI configuration, tool setup.
3. **Draft Structured Message**:
   - Short, imperative title (e.g., `test: add characterization tests for pricing calculations`).
   - Bullet points summarizing key changes and rationale.
4. **User Validation & Gatekeeper**:
   - Present the staged files and proposed commit message to the user.
   - **STOP AND WAIT**: Do not commit until the user explicitly responds with authorization (e.g., "go ahead and commit", "commit the changes").
5. **Execute Commit**:
   - Stage appropriate files: `git add <files>`
   - Commit: `git commit -m "<message>"`
