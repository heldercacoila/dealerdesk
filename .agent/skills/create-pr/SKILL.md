---
name: create-pr
description: Generates a structured Draft Pull Request from commit history using GitHub CLI. Must only be invoked via /Create-pr.
---

# Create Pull Request Skill

## Invocations & Strict Constraint

> [!IMPORTANT]
> **Strict Invocation Rule:** This skill can **ONLY** be invoked via the explicit slash command:
> `/Create-pr`
> 
> If the user requests a pull request using conversational plain text (e.g., "create the PR", "open a PR"), the agent **must reject the plain text request** and reply:
> *"Pull Request creation can only be executed via the `/Create-pr` command."*

---

## Operating Procedure

1. **Verify Branch and Prerequisite Checks**:
   - Ensure local branch is not `master`: `git branch --show-current`
   - Ensure working tree is clean: `git status --porcelain`
   - Ensure commits are pushed to remote.
2. **Analyze Branch Commit History**:
   - Retrieve all commits between `master` and the current branch:
     ```bash
     git log master..HEAD --oneline
     ```
   - Extract commit types (`feat:`, `test:`, `refactor:`, `docs:`, `chore:`, `fix:`) to compose the PR body.
3. **Generate PR Title and Body**:
   - Title format: `[Draft] <TASK_ID>: <Task Title>`
   - Body structure:
     - **Summary of Changes**: High-level context and rationale.
     - **Key Implementations**: Bullet points grouped by feature/infrastructure.
     - **Testing & Verification**: List of unit/regression/E2E tests executed.
     - **Known Issues & Next Steps**: Reference to `KNOWN_ISSUES.md` if applicable.
4. **Publish Draft PR via GitHub CLI (`gh`)**:
   - Run:
     ```bash
     gh pr create --draft --base master --title "<PR_TITLE>" --body "<PR_BODY>"
     ```
5. **Report PR Link**:
   - Output the generated PR URL returned by GitHub CLI.
