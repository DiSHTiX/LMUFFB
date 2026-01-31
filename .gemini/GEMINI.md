# GEMINI Agent Guidelines

## ⚠️ Critical Git Workflow Instruction

**NEVER** touch the git staging area.

*   **PROHIBITED COMMANDS:**
    *   `git add`
    *   `git commit`
    *   `git reset`
    *   `git push`
    *   `git restore --staged`

*   **NO EXCEPTIONS**: Do not stage files even if you created them or if you think it helps.
*   **The User** is exclusively responsible for reviewing and staging changes.
*   **Your Role:** Modify files on disk, run tests, verify the build, and then **stop**.


