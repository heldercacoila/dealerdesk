---
name: git-push
description: Pushes committed changes on the active branch to the remote repository after user confirmation.
---

# Git Push Skill

## Purpose

Pushes commits from the active local working branch to the remote git repository (`origin`). Can be invoked via natural language (e.g., "faz push", "envia para o remote") or direct command, either independently or following `git-commit`.

---

## Operating Procedure

1. **Verify Branch State**:
   - Determine current branch: `git branch --show-current`
   - Check ahead/behind status: `git status -sb`
2. **User Confirmation**:
   - Confirm branch name and destination with the user before pushing.
3. **Execute Push**:
   - Run: `git push -u origin <branch-name>`
4. **Report Result**:
   - Confirm push completion and remote tracking state.
