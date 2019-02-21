const users = [];

module.exports = {
  title: "Rainbow",
  tagline: "A website for testing",
  url: "https://tido64.github.io",
  baseUrl: "/rainbow/",
  projectName: "rainbow",
  organizationName: "tido64",
  headerLinks: [],
  users,
  headerIcon: "",
  footerIcon: "",
  favicon: "",
  colors: {
    primaryColor: "#212121",
    secondaryColor: "#b71c1c"
  },
  copyright: `Copyright © ${new Date().getFullYear()} Bifrost Entertainment AS and Tommy Nguyen`,
  highlight: {
    // Highlight.js theme to use for syntax highlighting in code blocks.
    theme: "rainbow"
  },
  scripts: ["https://buttons.github.io/buttons.js"],
  onPageNav: "separate",
  cleanUrl: true,
  customDocsPath: "docs/content",
  ogImage: "",
  twitterImage: "",
  repoUrl: "https://github.com/tido64/rainbow"
};
