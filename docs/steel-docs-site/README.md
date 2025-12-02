# Steel Docs Site

A simple, maintainable documentation site for the Steel programming language. One HTML shell + Markdown content loaded via hash routing.

## Structure

- `index.html`: Single-page shell (sidebar, topbar, content)
- `style/global-stylesheet.css`: Global look and feel
- `script/app.js`: Lightweight router + Markdown loader using `marked`
- `nav.json`: Sidebar/navigation config (titles, slugs, and file paths)
- `content/`: Markdown pages (one file per document)

## Run locally (Windows PowerShell)

Use one of the options below from this `docs` folder:

```powershell
# Prefer Python if installed
py -3 -m http.server 8000
# or
python -m http.server 8000

# Or use Node's serve
npx serve -l 8000
```

Then open: http://localhost:8000/

## Add a new page

1. Create a Markdown file in `content/`, e.g. `content/types.md`.
2. Add an entry in `nav.json` under the appropriate section:
   ```json
   {
     "title": "Types",
     "slug": "types",
     "path": "content/types.md"
   }
   ```
3. Navigate to `#/types` in the browser.

## Notes

- Update styling centrally in `style/global-stylesheet.css`.
- The sidebar is generated from `nav.json`, so you update navigation in one place.