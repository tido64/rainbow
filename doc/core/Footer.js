const React = require("react");

class Footer extends React.PureComponent {
  docUrl(doc, language) {
    const { baseUrl, docsUrl } = this.props.config;
    const docsPart = `${docsUrl ? `${docsUrl}/` : ""}`;
    return `${baseUrl}${docsPart}${doc}`;
  }

  render() {
    const { config, language } = this.props;
    return (
      <footer className="nav-footer" id="footer">
        <section className="sitemap">
          <a href={config.baseUrl} className="nav-home">
            {config.footerIcon && (
              <img
                src={config.baseUrl + config.footerIcon}
                alt={config.title}
                width="66"
                height="58"
              />
            )}
          </a>
          <div>
            <h5>Docs</h5>
            <a href={this.docUrl("getting-started", language)}>
              Getting Started (or other categories)
            </a>
            <a href={this.docUrl("coding-standard", language)}>
              Coding Standard
            </a>
          </div>
          <div>
            <h5>Community</h5>
            <a href={"https://github.com/tido64/rainbow/issues"}>Issues</a>
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
  }
}

module.exports = Footer;
