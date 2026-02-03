# Guide: Archiving GitHub Issues Locally

This guide outlines the process for creating local Markdown copies of GitHub issues to preserve context, offline availability, and project history. It specifically addresses challenges with detecting and downloading embedded images.

## Standard Workflow

1.  **Create the Directory Structure**
    Ensure a dedicated directory exists for issues and their assets:
    ```bash
    mkdir -p docs/github_issues/images
    ```

2.  **Fetch the Text Content**
    *   Use a web scraping tool or manually copy the issue description and comments.
    *   **Naming Convention:** `docs/github_issues/issue_<NUMBER>_<SHORT_DESCRIPTION>.md`
    *   **Format:**
        *   Title: Issue Title
        *   Link: Original GitHub URL
        *   Body: Description
        *   Comments: Chronological order with author names.

3.  **Localize Links**
    *   Scan the text for links to files *already* in the repository (e.g., `blob/main/docs/...`).
    *   Convert absolute GitHub URLs to relative local paths (e.g., `../dev_docs/architecture.md`) to ensure navigation works offline.

## ⚠️ CRITICAL: Handling Images

GitHub issues often contain images hosted on `github.com/user-attachments/assets/...`. Standard text-based scraping tools (like `web_fetch` or simple "Reader Mode" views) often **fail to extract these URLs**, displaying them only as `[Image]` or omitting them entirely because they are dynamically rendered or hidden in the HTML structure.

### How to Detect Missing Images
*   **Context Clues:** Look for text like "See screenshot below", "As shown here:", or empty gaps in the narrative.
*   **Placeholders:** If your scraper returns generic text like `[Image]` or `![Image]`, the URL was likely lost.

### The "Raw HTML" Technique (Reliable Method)
If you suspect an image is missing or cannot find the URL, **do not rely on the rendered text**. You must inspect the raw HTML.

**1. Download the Raw HTML**
Use PowerShell to download the full page source to a temporary file.
```powershell
Invoke-WebRequest -Uri "https://github.com/USERNAME/REPO/issues/NUMBER" -OutFile "issue_temp.html"
```

**2. Extract Image URLs**
Search the HTML file for GitHub's asset pattern (`user-attachments` or common extensions).
```powershell
Select-String -Path "issue_temp.html" -Pattern "user-attachments" -Context 2
# OR
Select-String -Path "issue_temp.html" -Pattern "\.png|\.jpg|\.jpeg"
```
*Look for URLs starting with `https://github.com/user-attachments/assets/...` or `https://private-user-images.githubusercontent.com/...`*

**3. Download the Image**
Once you have the specific URL:
```powershell
Invoke-WebRequest -Uri "IMAGE_URL_FROM_STEP_2" -OutFile "docs/github_issues/images/issue_NUMBER_description.png"
```

**4. Link in Markdown**
Add the image to your markdown file using standard syntax:
```markdown
![Description of image](images/issue_NUMBER_description.png)
```

**5. Clean Up**
Remove the temporary HTML file.
```powershell
rm issue_temp.html
```

## Checklist
- [ ] Issue content copied to `docs/github_issues/`.
- [ ] Internal links converted to relative paths.
- [ ] **Images checked:** Raw HTML scanned for hidden `user-attachments`.
- [ ] Images downloaded to `images/` subfolder.
- [ ] Markdown updated to link local images.
