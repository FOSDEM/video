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

  const mclient: MisirkaClient = (() => {
    const params = new URLSearchParams(window.location.hash.slice(1))
    const mqtt_url = params.get("mqtt_url")

    if (mqtt_url) {
      const mqtt_client = new MQTTClient({
        mqtt_url: mqtt_url,
        prefix: "/fosdem/",
      })

      mqtt_client.on_alive(() => {
        mixer_el.textContent = 'MQTT connected, waiting for audioctl to appear'
      })

      mqtt_client.on_dead(() => {
        mixer_el.textContent = 'MQTT disconnected, reconnecting'
      })

      return new SubClient(mqtt_client, {
        prefix: "audioctl/",
        online_topic: "audioctl/online",
      })
    } else {
      return new WSClient({ ws_url: ws_url() })
    }
  })()

  const ui = new MixerUI(mclient, mixer_el)

  document.querySelector<HTMLInputElement>('#setup-mode')!.onchange =
    e => ui.toggleSetupMode((e.target as HTMLInputElement)!.checked)
}

function ws_url() {
  const protocol = location.protocol === "https:" ? "wss" : "ws"
  return `${protocol}://${location.host}${location.pathname.replace(/\/?$/, '/ws')}`
}

main()
