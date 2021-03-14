import React from "react";
import clsx from "clsx";
import CodeBlock from "@theme/CodeBlock";
import Layout from "@theme/Layout";
import Link from "@docusaurus/Link";
import useDocusaurusContext from "@docusaurus/useDocusaurusContext";
import useBaseUrl from "@docusaurus/useBaseUrl";
import styles from "./styles.module.css";

const features = [
  {
    title: "Example: Label",
    imageUrl: "img/hello-world.png",
    description: (
      <CodeBlock className="language-js">
        {[
          `const label = new Rainbow.Label()`,
          `  .alignment(Rainbow.TextAlignment.Center)`,
          `  .font("OpenSans-Light.ttf")`,
          `  .fontSize(60)`,
          `  .position({`,
          `    x: screenWidth * 0.5,`,
          `    y: screenHeight * 0.5,`,
          `  })`,
          `  .text("Hello\\nWorld");`,
          ``,
          `Rainbow.RenderQueue.add(label);`,
        ].join("\n")}
      </CodeBlock>
    ),
  },
  {
    title: "Example: Sprite Sheet Animation",
    imageUrl: "img/sprite-sheet-animations-example.gif",
    description: (
      <CodeBlock className="language-js">
        {[
          `const texture = new Rainbow.Texture("monkey.png");`,
          ``,
          `const batch = new Rainbow.SpriteBatch(1);`,
          `batch.setTexture(texture);`,
          ``,
          `const sprite = batch.createSprite(104, 149);`,
          `sprite.position({`,
          `  x: screenWidth * 0.5,`,
          `  y: screenHeight * 0.5,`,
          `});`,
          ``,
          `const frames = [`,
          `  { left: 400, bottom: 724, width: 104, height: 149 },`,
          `  { left: 504, bottom: 724, width: 104, height: 149 },`,
          `  { left: 608, bottom: 724, width: 104, height: 149 },`,
          `  { left: 712, bottom: 724, width: 104, height: 149 },`,
          `  { left: 816, bottom: 724, width: 104, height: 149 },`,
          `  { left: 920, bottom: 724, width: 104, height: 149 },`,
          `];`,
          ``,
          `const animation = new Rainbow.Animation(`,
          `  sprite,`,
          `  frames,`,
          `  6,`,
          `  0);`,
          `animation.start();`,
          ``,
          `Rainbow.RenderQueue.add(batch);`,
          `Rainbow.RenderQueue.add(animation);`,
        ].join("\n")}
      </CodeBlock>
    ),
  },
];

function Feature({ imageUrl, title, description }) {
  const imgUrl = useBaseUrl(imageUrl);
  return (
    <div className={clsx("col col--6", styles.feature)}>
      {imgUrl && (
        <div className="text--center">
          <img className={styles.featureImage} src={imgUrl} alt={title} />
        </div>
      )}
      <h3>{title}</h3>
      <p>{description}</p>
    </div>
  );
}

export default function Home() {
  const context = useDocusaurusContext();
  const { siteConfig = {} } = context;
  return (
    <Layout
      title="Home"
      description="Rainbow is a scriptable, cross-platform, 2D game engine."
    >
      <header className={clsx("hero hero--primary", styles.heroBanner)}>
        <div className="container">
          <h1 className="hero__title">{siteConfig.title}</h1>
          <p className="hero__subtitle">{siteConfig.tagline}</p>
          <div className={styles.buttons}>
            <Link
              className={clsx(
                "button button--outline button--secondary button--lg",
                styles.getStarted
              )}
              to={useBaseUrl("docs/getting-started/")}
            >
              Get Started
            </Link>
          </div>
        </div>
      </header>
      <main>
        {features && features.length > 0 && (
          <section className={styles.features}>
            <div className="container">
              <div className="row">
                {features.map((props, idx) => (
                  <Feature key={idx} {...props} />
                ))}
              </div>
            </div>
          </section>
        )}
      </main>
    </Layout>
  );
}
