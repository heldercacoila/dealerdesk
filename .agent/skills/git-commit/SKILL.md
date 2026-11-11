---
name: git-commit
description: Analyzes git status and diff, creates a standardized Conventional Commit, and requests user confirmation before committing.
---

# Git Commit Skill

## Purpose

Standardize atomic git commits following the Conventional Commits specification. Can be invoked via natural language (e.g., "faz commit da fase 1", "faz commit das alterações") or direct command.

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
4. **User Validation**:
   - Present the staged files and proposed commit message to the user.
5. **Execute Commit**:
   - Stage appropriate files: `git add <files>`
   - Commit: `git commit -m "<message>"`
