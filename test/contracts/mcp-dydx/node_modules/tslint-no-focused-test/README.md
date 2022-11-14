# 🔬 tslint-no-focused-test

This is a custom [TSLint](https://palantir.github.io/tslint/) rule that checks
for the following focused tests:

* `fit`
* `fdescribe`
* `it.only`
* `describe.only`
* `context.only`
* `test.only`

The intended use case is as a pre-commit hook or build (CI/CD) task.

### 📝 Install

Install the package with

`npm install tslint-no-focused-test --save-dev` (or `yarn add tslint-no-focused-test --save-dev`).

Then add the following to your `tslint.json`:

```
  "rulesDirectory": [
    "tslint-no-focused-test"
  ],
  "rules": {
    "no-focused-test": true
  }
```

### 📖 Examples

#### ↩️ Pre-commit hook example

Install `husky` and `lint-staged` and save to development dependencies. Then
configure in your `package.json`:

```
  "scripts": {
    "precommit": "lint-staged"
  },
  "lint-staged": {
    "*.ts": ["tslint -c tslint.json 'test/**/*.test.ts'"]
  }
```

#### 📦  Build task example

E.g. as a step in a job for a CircleCI build:

```
version: 2
jobs:
  build-app:
    docker:
      - image: node:6
    steps:
      - run:
          name: lint:tests
          command: npm run lint:tests
```

where the `"scripts"` option of the `package.json` has

`"lint:tests": "tslint -c tslint.json 'test/**/*.test.ts'"`
