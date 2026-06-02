import { defineConfig } from "vite"
import { viteSingleFile } from "vite-plugin-singlefile"
import viteCompression from "vite-plugin-compression"
import checker from 'vite-plugin-checker'

export default defineConfig(({ mode }) => {
  let server_cfg = {}
  let optimize_deps = {
    exclude: new Array<string>(),
    include: ['buffer'],  // used by MQTT.js
  }

  if (mode === "development") {
    server_cfg = {
      watch: {
        ignored: ['!**/node_modules/misirka/**'],
      },
    }

    optimize_deps.exclude.push('misirka')
  }

  const checker_cfg = {
    typescript: true,
  }

  return {
    optimizeDeps: optimize_deps,
    plugins: [viteSingleFile(), viteCompression(), checker(checker_cfg)],
    build: {
      assetsInlineLimit: Number.MAX_SAFE_INTEGER,
    },
    server: server_cfg,
  }
})
