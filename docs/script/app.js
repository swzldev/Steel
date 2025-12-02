(() => {
  const state = {
    nav: null,
    routes: new Map(), // slug -> { title, path }
    home: 'index'
};

  const $ = (sel) => document.querySelector(sel);
  const navEl = $('#nav');
  const docEl = $('#doc');
  const yearEl = $('#year');
  const searchEl = $('#quickSearch');
  const sidebarEl = $('#sidebar');
  const menuToggle = $('#menuToggle');
  const breadcrumbsEl = $('#breadcrumbs');

  yearEl.textContent = String(new Date().getFullYear());

  if (menuToggle) {
    menuToggle.addEventListener('click', () => {
      sidebarEl.classList.toggle('open');
    });
  }

  window.addEventListener('hashchange', () => {
    navigate(location.hash);
  });

  // Simple debounce
  const debounce = (fn, ms = 150) => {
    let t; return (...args) => { clearTimeout(t); t = setTimeout(() => fn(...args), ms); };
  };

  // Quick filter (client-side) for links
  if (searchEl) {
    const doFilter = debounce(() => {
      const q = searchEl.value.trim().toLowerCase();
      const links = navEl.querySelectorAll('a.nav-link');
      links.forEach(a => {
        const match = a.textContent.toLowerCase().includes(q);
        a.parentElement.style.display = match ? '' : 'none';
      });
    }, 80);
    searchEl.addEventListener('input', doFilter);
  }

  function buildItem(item, ancestors) {
    const li = document.createElement('li');
    li.className = 'nav-item';

    // Register route if item has a path/slug
    if (item.slug && item.path) {
      state.routes.set(item.slug, { title: item.title, path: item.path });
      // Build breadcrumb list for this slug
      const crumb = [...(ancestors || []), { title: item.title, slug: item.slug }];
      if (!state.breadcrumbs) state.breadcrumbs = new Map();
      state.breadcrumbs.set(item.slug, crumb);
    }

    const a = document.createElement('a');
    a.className = 'nav-link';
    a.textContent = item.title;
    if (item.slug) a.href = `#/${item.slug}`;
    li.appendChild(a);

    if (Array.isArray(item.children) && item.children.length) {
      li.classList.add('has-children');
      const sub = document.createElement('ul');
      sub.className = 'nav-list';
      const nextAncestors = [...(ancestors || []), { title: item.title, slug: item.slug }];
      item.children.forEach(child => {
        sub.appendChild(buildItem(child, nextAncestors));
      });
      li.appendChild(sub);
    }
    return li;
  }

  async function loadNav() {
    try {
      const res = await fetch('nav.json');
      const nav = await res.json();
      state.nav = nav;
      state.home = nav.home || 'index';

      // Build routes lookup and sidebar
      navEl.innerHTML = '';
      nav.sections.forEach(section => {
        const sec = document.createElement('div');
        sec.className = 'nav-section';
        const title = document.createElement('div');
        title.className = 'nav-section-title';
        title.textContent = section.title;
        const list = document.createElement('ul');
        list.className = 'nav-list';

        const baseAncestors = [{ title: section.title, slug: null }];
        (section.items || []).forEach(item => {
          list.appendChild(buildItem(item, baseAncestors));
        });

        sec.appendChild(title);
        sec.appendChild(list);
        navEl.appendChild(sec);
      });
    } catch (err) {
      console.error('Failed to load nav.json', err);
      navEl.innerHTML = '<div class="callout">Failed to load navigation.</div>';
    }
  }

  function setActiveLink(slug) {
    const links = navEl.querySelectorAll('a.nav-link');
    links.forEach(a => a.classList.remove('active'));
    navEl.querySelectorAll('.nav-item').forEach(li => li.classList.remove('active-parent'));
    const active = navEl.querySelector(`a.nav-link[href="#/${CSS.escape(slug)}"]`);
    if (active) {
      active.classList.add('active');
      // mark parents for subtle styling
      let p = active.parentElement;
      while (p && p !== navEl) {
        if (p.classList.contains('nav-item')) p.classList.add('active-parent');
        p = p.parentElement;
      }
    }
  }

  function renderBreadcrumb(slug) {
    if (!breadcrumbsEl) return;
    const crumbs = (state.breadcrumbs && state.breadcrumbs.get(slug)) || [];
    // Build breadcrumb DOM: Section (no link) › Parent(s) (link) › Current (no link, accent)
    breadcrumbsEl.innerHTML = '';
    const frag = document.createDocumentFragment();
    crumbs.forEach((c, idx) => {
      const isLast = idx === crumbs.length - 1;
      const span = document.createElement('span');
      span.className = 'crumb' + (isLast ? ' current' : '');
      if (!isLast && c.slug) {
        const a = document.createElement('a');
        a.href = `#/${c.slug}`;
        a.textContent = c.title;
        span.appendChild(a);
      } else {
        span.textContent = c.title;
      }
      frag.appendChild(span);
      if (!isLast) {
        const sep = document.createElement('span');
        sep.className = 'sep';
        sep.setAttribute('aria-hidden', 'true');
        sep.textContent = '›';
        frag.appendChild(sep);
      }
    });
    breadcrumbsEl.appendChild(frag);
  }

  async function renderMarkdown(mdText) {
    if (window.marked) {
      marked.setOptions({ mangle: false, headerIds: true });
      const html = marked.parse(mdText);
      docEl.innerHTML = html;
      if (window.hljs) {
        docEl.querySelectorAll('pre code').forEach(el => hljs.highlightElement(el));
      }
    } else {
      docEl.textContent = mdText;
    }
  }

  async function navigate(hash) {
    const slug = (hash || '').replace(/^#\/?/, '') || state.home;
    // Close sidebar on mobile when navigating
    sidebarEl.classList.remove('open');

    const route = state.routes.get(slug);
    setActiveLink(slug);
    renderBreadcrumb(slug);
    if (!route) {
      document.title = `Steel Docs — Not Found`;
      docEl.innerHTML = `<h1>Not Found</h1><p>The page <code>${slug}</code> does not exist.</p>`;
      return;
    }

    try {
      docEl.innerHTML = '<div class="callout">Loading…</div>';
      const res = await fetch(route.path);
      if (!res.ok) throw new Error(res.status + ' ' + res.statusText);
      const text = await res.text();
      await renderMarkdown(text);
      document.title = `Steel Docs — ${route.title}`;
      docEl.scrollTo({ top: 0, behavior: 'instant' });
    } catch (err) {
      console.error('Failed to load doc', err);
      document.title = `Steel Docs — Error`;
      docEl.innerHTML = `<h1>Error</h1><p>Could not load this document.</p>`;
    }
  }

  (async function init() {
    await loadNav();
    if (!location.hash) location.hash = `#/${state.home}`;
    navigate(location.hash);
  })();
})();
