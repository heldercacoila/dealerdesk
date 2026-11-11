---
name: git-push
description: Pushes committed changes on the active branch to the remote repository after user confirmation.
---

# Git Push Skill

## Critical Constraint

> [!CAUTION]
> **Zero Autonomous Pushes**: The agent is **strictly forbidden** from running `git push` without explicit, unambiguous user authorization. Always confirm branch and remote, then wait for user approval.

---

## Purpose

Pushes commits from the active local working branch to the remote git repository (`origin`). Can be invoked via natural language (e.g., "push changes", "push to remote") or direct command, either independently or following `git-commit`.

---

## Operating Procedure

1. **Verify Branch State**:
   - Determine current branch: `git branch --show-current`
   - Check ahead/behind status: `git status -sb`
2. **User Confirmation & Gatekeeper**:
   - Confirm branch name and destination with the user before pushing.
   - **STOP AND WAIT**: Do not push until the user explicitly responds with authorization (e.g., "you can push now", "push to remote").
3. **Execute Push**:
   - Run: `git push -u origin <branch-name>`
4. **Report Result**:
   - Confirm push completion and remote tracking state.
