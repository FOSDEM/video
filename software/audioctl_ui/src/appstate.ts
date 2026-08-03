import { MisirkaClient, MQTTClient, SubClient, WSClient } from "misirka"
import { useEffect, useState } from "preact/hooks"

export const status_messages = {
  initialising: "initialising",
  mqtt_connecting: "connecting to MQTT",
  mqtt_connected_waiting_for_audioctl:
    "MQTT connected, waiting for audioctl to appear",
  mqtt_disconnected: "MQTT disconnected, reconnecting",
  audioctl_died:
    "MQTT still connected but audioctl died, waiting for it to appear",
  ws_connecting: "connecting to websocket",
  ws_disconnected: "websocket disconnected, reconnecting",
} as const

type StatusMessage = keyof typeof status_messages

type AppState =
  | { status: "showing_help" }
  | { status: "showing_msg"; msg: StatusMessage }
  | { status: "connected"; client: MisirkaClient }

export default function useAppState() {
  const [appState, setAppState] = useState<AppState>({
    status: "showing_msg",
    msg: "initialising",
  })
  useEffect(() => {
    function connect() {
      const params = new URLSearchParams(window.location.search)
      const mqtt_url = params.get("mqtt_url")
      const ws_url = params.get("ws_url")

      if (mqtt_url) {
        let mqtt_prefix = params.get("mqtt_prefix")
        if (!mqtt_prefix) {
          mqtt_prefix = "/fosdem/"
        }

        const mqtt_client = new MQTTClient({
          mqtt_url: mqtt_url,
          prefix: mqtt_prefix,
        })

        setAppState({ status: "showing_msg", msg: "mqtt_connecting" })

        mqtt_client.on_alive(() => {
          setAppState({
            status: "showing_msg",
            msg: "mqtt_connected_waiting_for_audioctl",
          })
        })

        mqtt_client.on_dead(() => {
          setAppState({ status: "showing_msg", msg: "mqtt_disconnected" })
        })

        const client = new SubClient(mqtt_client, {
          prefix: "audioctl/",
          online_topic: "audioctl/online",
        })

        client.on_alive(() => {
          setAppState({ status: "connected", client })
        })

        client.on_dead(() => {
          setAppState({ status: "showing_msg", msg: "audioctl_died" })
        })
      } else if (ws_url) {
        const client = new WSClient({ ws_url: ws_url })

        setAppState({ status: "showing_msg", msg: "ws_connecting" })

        client.on_alive(() => {
          setAppState({ status: "connected", client })
        })

        client.on_dead(() => {
          setAppState({ status: "showing_msg", msg: "ws_disconnected" })
        })
      } else {
        setAppState({ status: "showing_help" })
      }
    }

    connect()

    // return () => disconnect();
  }, [])

  return appState
}
