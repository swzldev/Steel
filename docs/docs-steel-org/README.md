# Steel Docs Site

A simple, maintainable documentation site for the Steel programming language. One HTML shell + Markdown content loaded via hash routing.

## Viewing

To view the site, download the code and open `index.html` in your browser of choice (or host it on a local server).

Alternatively, you can browse through the `content` folder and read the markdown files directly in GitHub's markdown viewer.

## Structure

- `index.html`: Single-page shell (sidebar, topbar, content)
- `style/global-stylesheet.css`: Global look and feel
- `script/app.js`: Lightweight router + Markdown loader using `marked`
- `nav.json`: Sidebar/navigation config (titles, slugs, and file paths)
- `content/`: Markdown pages (one file per document)

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