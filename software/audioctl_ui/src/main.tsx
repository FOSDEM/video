import favicon_dataurl from "../assets/favicon.png?url&inline"
import { render } from "preact"
import type { VNode } from "preact"
import { MixerUI } from "./mixer_ui.tsx"
import useAppState, { status_messages } from "./appstate.ts"
import { linkify, mqtt_dflt_url, ws_dflt_url } from "./helpers.tsx"
import { ErrHandlerCtx } from "./err_context.ts"

import "preact/debug"

function setup_favicon() {
  const link = document.querySelector<HTMLLinkElement>("#favicon")!
  link.rel = "icon"
  link.type = "image/png"
  link.href = favicon_dataurl
}

function App() {
  const appState = useAppState()

  console.log(appState.status)

  return (
    <ErrHandlerCtx.Provider value={action_err_handler}>
      {appState.status == "connected" && <MixerUI client={appState.client} />}
      {appState.status == "showing_help" && help_message()}
      {appState.status == "showing_msg" && (
        <section>{status_messages[appState.msg]}</section>
      )}
    </ErrHandlerCtx.Provider>
  )
}

const action_err_handler = async (err: any) => {
  console.error("UI action failed to execute: ", err)
  alert("Action failed")
}

function help_message(): VNode {
  return (
    <section>
      You need to specify a way to connect to the backend as a query param. Here
      are some examples:
      <ul>
        <li>Direct websocket: {linkify(ws_dflt_url())}</li>
        <li>MQTT: {linkify(mqtt_dflt_url())}</li>
      </ul>
    </section>
  )
}

function main() {
  setup_favicon()

  const mixer_el = document.querySelector<HTMLDivElement>("#mixer")!
  render(<App />, mixer_el)
}

main()
