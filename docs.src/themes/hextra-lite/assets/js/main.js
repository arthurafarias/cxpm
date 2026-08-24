(function () {
  "use strict";

  /* ---- mobile sidebar ---- */
  var shell = document.querySelector(".shell");
  var toggle = document.getElementById("menu-toggle");
  var backdrop = document.getElementById("sidebar-backdrop");

  function closeSidebar() {
    if (!shell) return;
    shell.classList.remove("sidebar-open");
    if (toggle) toggle.setAttribute("aria-expanded", "false");
  }

  if (toggle && shell) {
    toggle.addEventListener("click", function () {
      var open = shell.classList.toggle("sidebar-open");
      toggle.setAttribute("aria-expanded", open ? "true" : "false");
    });
  }
  if (backdrop) backdrop.addEventListener("click", closeSidebar);
  document.addEventListener("keydown", function (e) {
    if (e.key === "Escape") closeSidebar();
  });

  /* ---- theme toggle ---- */
  var themeToggle = document.getElementById("theme-toggle");
  if (themeToggle) {
    themeToggle.addEventListener("click", function () {
      var root = document.documentElement;
      var current = root.getAttribute("data-theme") === "dark" ? "dark" : "light";
      var next = current === "dark" ? "light" : "dark";
      root.setAttribute("data-theme", next);
      try { localStorage.setItem("cxpm-docs-theme", next); } catch (e) {}
    });
  }

  /* ---- search ---- */
  var input = document.getElementById("search-input");
  var results = document.getElementById("search-results");
  var index = null;
  var indexPromise = null;

  function loadIndex() {
    if (!indexPromise) {
      var url = (window.CXPM_DOCS && window.CXPM_DOCS.searchIndexURL) || "/search-index.json";
      indexPromise = fetch(url).then(function (r) { return r.json(); }).then(function (data) {
        index = data;
        return data;
      });
    }
    return indexPromise;
  }

  function escapeHTML(s) {
    return s.replace(/[&<>"']/g, function (c) {
      return { "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c];
    });
  }

  function render(items, query) {
    if (!items.length) {
      results.innerHTML = '<div class="no-results">No results for “' + escapeHTML(query) + '”</div>';
      results.hidden = false;
      return;
    }
    var html = items.slice(0, 8).map(function (item) {
      return '<a href="' + item.href + '">' +
        '<div class="result-title">' + escapeHTML(item.title) + '</div>' +
        '<div class="result-snippet">' + escapeHTML(item.content.slice(0, 120)) + '…</div>' +
        '</a>';
    }).join("");
    results.innerHTML = html;
    results.hidden = false;
  }

  function search(query) {
    if (!query) {
      results.hidden = true;
      results.innerHTML = "";
      return;
    }
    loadIndex().then(function (data) {
      var q = query.toLowerCase();
      var matches = data.filter(function (item) {
        return item.title.toLowerCase().indexOf(q) !== -1 ||
          item.content.toLowerCase().indexOf(q) !== -1;
      });
      render(matches, query);
    });
  }

  if (input) {
    input.addEventListener("focus", loadIndex);
    input.addEventListener("input", function () { search(input.value.trim()); });
    document.addEventListener("click", function (e) {
      if (!e.target.closest(".search")) {
        results.hidden = true;
      }
    });
    document.addEventListener("keydown", function (e) {
      if (e.key === "/" && document.activeElement !== input && !/^(INPUT|TEXTAREA)$/.test(document.activeElement.tagName)) {
        e.preventDefault();
        input.focus();
      }
    });
  }
})();
