/** @type {import("@docusaurus/types").DocusaurusConfig} */
module.exports = {
  title: "Rainbow",
  tagline: "Rainbow is a scriptable, cross-platform, 2D game engine.",
  url: "https://nguyen.works/rainbow",
  baseUrl: "/rainbow/",
  onBrokenLinks: "throw",
  onBrokenMarkdownLinks: "warn",
  favicon: "img/favicon.ico",
  organizationName: "tido64", // Usually your GitHub org/user name.
  projectName: "rainbow", // Usually your repo name.
  themeConfig: {
    navbar: {
      title: "Rainbow",
      logo: {
        alt: "Rainbow Logo",
        src: "img/logo.png",
      },
      items: [
        {
          to: "docs/getting-started/",
          activeBasePath: "docs",
          label: "Docs",
          position: "left",
        },
        {
          href: "https://github.com/tido64/rainbow",
          label: "GitHub",
          position: "right",
        },
      ],
    },
    footer: {
      style: "dark",
      links: [
        {
          title: "Docs",
          items: [
            {
              label: "Getting Started",
              to: "docs/getting-started/",
            },
            {
              label: "Coding Standard",
              to: "docs/coding-standard/",
            },
          ],
        },
        {
          title: "Community",
          items: [
            {
              label: "Discussions",
              href: "https://github.com/tido64/rainbow/discussions",
            },
          ],
        },
        {
          title: "More",
          items: [
            {
              label: "GitHub",
              href: "https://github.com/tido64/rainbow",
            },
          ],
        },
      ],
      copyright: `Copyright © ${new Date().getFullYear()} Bifrost Entertainment AS and Tommy Nguyen. Built with Docusaurus.`,
    },
  },
  presets: [
    [
      "@docusaurus/preset-classic",
      {
        docs: {
          sidebarPath: require.resolve("./sidebars.js"),
          editUrl: "https://github.com/tido64/rainbow/edit/master/doc/",
        },
        blog: {
          showReadingTime: true,
          editUrl: "https://github.com/tido64/rainbow/edit/master/doc/blog/",
        },
        theme: {
          customCss: require.resolve("./src/css/custom.css"),
        },
      },
    ],
  ],
};
