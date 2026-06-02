import js from "@eslint/js"
import globals from "globals"
import tseslint from "typescript-eslint"
import css from "@eslint/css"
import { defineConfig, globalIgnores } from "eslint/config"

export default defineConfig([
  globalIgnores([
    "!node_modules/",
    "node_modules/*",
    "!node_modules/misirka/",
  ]),
  {
    files: ["**/*.{js,mjs,cjs,ts,mts,cts,tsx,jsx}"],
    plugins: { js },
    extends: ["js/recommended"],
    languageOptions: { globals: globals.browser },
    rules: {
      "indent": ["error", 2],
      "semi": ["error", "never"],
      "no-unused-vars": "off",
      "comma-dangle": ["error", "always-multiline"],
    },
  },
  {
    extends: [tseslint.configs.recommended],
    rules: {
      "@typescript-eslint/no-unused-vars": "off",
      "@typescript-eslint/no-explicit-any": "off",
    },
  },
  {
    files: ["**/*.css"],
    plugins: { css },
    language: "css/css",
    extends: ["css/recommended"],
  },
])
