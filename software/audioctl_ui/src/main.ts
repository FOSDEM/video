import favicon_dataurl from "../assets/favicon.png?url&inline"
import { MixerUI } from "./mixer_ui.tsx"
import { MQTTClient, WSClient, MisirkaClient, SubClient } from "misirka"

function setup_favicon() {
  const link = document.querySelector<HTMLLinkElement>("#favicon")!
  link.rel = "icon"
  link.type = "image/png"
  link.href = favicon_dataurl
}

function main() {
  setup_favicon()

  const mixer_el = document.querySelector<HTMLDivElement>('#mixer')!

  const mclient: MisirkaClient | null = (() => {
    const params = new URLSearchParams(window.location.search)
    const mqtt_url = params.get("mqtt_url")
    const ws_url = params.get("ws_url")

    if (mqtt_url) {
      let mqtt_prefix = params.get("mqtt_prefix")
      if (!mqtt_prefix) {
        mqtt_prefix = '/fosdem/'
      }

      const mqtt_client = new MQTTClient({
        mqtt_url: mqtt_url,
        prefix: mqtt_prefix,
      })

      mixer_el.textContent = 'connecting to MQTT'

      mqtt_client.on_alive(() => {
        mixer_el.textContent = 'MQTT connected, waiting for audioctl to appear'
      })

      mqtt_client.on_dead(() => {
        mixer_el.textContent = 'MQTT disconnected, reconnecting'
      })

      const mclient = new SubClient(mqtt_client, {
        prefix: "audioctl/",
        online_topic: "audioctl/online",
      })

      mclient.on_dead(() => {
        mixer_el.textContent = 'MQTT still connected but audioctl died, waiting for it to appear'
      })

      return mclient
    } else if (ws_url) {
      const mclient = new WSClient({ ws_url: ws_url })

      mixer_el.textContent = 'connecting to websocket'

      mclient.on_dead(() => {
        mixer_el.textContent = 'websocket disconnected, reconnecting'
      })

      return mclient
    } else {
      mixer_el.innerHTML = `
        <section>
          You need to specify a way to connect to the backend as a query param.
          Here are some examples:
          <ul>
            <li>Direct websocket: ${linkify(ws_dflt_url())}</li>
            <li>MQTT: ${linkify(mqtt_dflt_url())}</li>
          </ul>
        </section>
      `
      return null
    }
  })()

  if (!mclient) {
    return
  }

  new MixerUI(mclient, mixer_el)
}

function linkify(l: string): string {
  return `<a href="${l}">${l}</a>`
}

function ws_dflt_url() {
  return `${location.protocol}//${location.host}${location.pathname}?ws_url=http://localhost:8811/ws`
}

function mqtt_dflt_url(): string {
  return `${location.protocol}//${location.host}${location.pathname}?mqtt_url=http://localhost:1880&mqtt_prefix=/fosdem/`
}

main()
