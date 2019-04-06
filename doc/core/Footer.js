// @ts-check
const React = require("react");

/**
 * Returns the URL for specified doc page.
 * @typedef {{
 *     title: string;
 *     baseUrl: string;
 *     copyright: string;
 *     repoUrl: string;
 *     docsUrl: string;
 *     footerIcon?: string
 * }} DocusaurusConfig
 * @param {string} doc
 * @param {DocusaurusConfig} config
 * @param {string} language
 * @returns {string}
 */
function docUrl(doc, config, language) {
  const { baseUrl, docsUrl } = config;
  const docsPart = `${docsUrl ? `${docsUrl}/` : ""}`;
  return `${baseUrl}${docsPart}${doc}`;
}

/**
 * Documentation footer.
 * @typedef {{
 *   config: DocusaurusConfig;
 *   language: string;
 * }} FooterProps
 * @param {FooterProps} props
 */
function Footer(props) {
  const { config, language } = props;
  return (
    <footer className="nav-footer" id="footer">
      <section className="sitemap">
        <a href={config.baseUrl} className="nav-home">
          {config.footerIcon && (
            <img
              src={config.baseUrl + config.footerIcon}
              alt={config.title}
              width="52"
              height="52"
            />
          )}
        </a>
        <div>
          <h5>Docs</h5>
          <a href={docUrl("getting-started", config, language)}>
            Getting Started (or other categories)
          </a>
          <a href={docUrl("coding-standard", config, language)}>
            Coding Standard
          </a>
        </div>
        <div>
          <h5>Community</h5>
          <a href="https://www.reddit.com/r/rainbowtech">Forum</a>
          <a href="https://github.com/tido64/rainbow/issues">Issues</a>
          <a href="https://github.com/tido64/rainbow/pulls">Pull Requests</a>
          <a href="https://trello.com/b/r2TqudY6/rainbow">Roadmap</a>
        </div>
        <div>
          <h5>More</h5>
          <a href="https://github.com/tido64/rainbow">GitHub</a>
          <a
            className="github-button"
            href={config.repoUrl}
            data-icon="octicon-star"
            data-show-count="true"
            data-count-aria-label="# stargazers on GitHub"
            aria-label="Star this project on GitHub"
          >
            Star
          </a>
        </div>
      </section>
      <section className="copyright">{config.copyright}</section>
    </footer>
  );
};

module.exports = Footer;
