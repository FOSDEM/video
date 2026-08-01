import favicon_dataurl from "../assets/favicon.png?url&inline";
import { render, Component } from "preact";
import type { VNode } from "preact";
import { MQTTClient, WSClient, MisirkaClient, SubClient } from "misirka";
import { MixerUI } from "./mixer_ui.tsx";

function setup_favicon() {
  const link = document.querySelector<HTMLLinkElement>("#favicon")!;
  link.rel = "icon";
  link.type = "image/png";
  link.href = favicon_dataurl;
}

const status_messages = {
  initialising: "initialising",
  mqtt_connecting: "connecting to MQTT",
  mqtt_connected_waiting_for_audioctl:
    "MQTT connected, waiting for audioctl to appear",
  mqtt_disconnected: "MQTT disconnected, reconnecting",
  audioctl_died:
    "MQTT still connected but audioctl died, waiting for it to appear",
  ws_connecting: "connecting to websocket",
  ws_disconnected: "websocket disconnected, reconnecting",
} as const;

type StatusMessage = keyof typeof status_messages;

type AppState =
  | { status: "showing_help" }
  | { status: "showing_msg"; msg: StatusMessage }
  | { status: "connected"; client: MisirkaClient };

class App extends Component<object, AppState> {
  state: AppState = { status: "showing_msg", msg: "initialising" };

  componentDidMount() {
    this.connect();
  }

  private setAppState(state: AppState) {
    this.setState(state);
  }

  private connect() {
    const params = new URLSearchParams(window.location.search);
    const mqtt_url = params.get("mqtt_url");
    const ws_url = params.get("ws_url");

    if (mqtt_url) {
      let mqtt_prefix = params.get("mqtt_prefix");
      if (!mqtt_prefix) {
        mqtt_prefix = "/fosdem/";
      }

      const mqtt_client = new MQTTClient({
        mqtt_url: mqtt_url,
        prefix: mqtt_prefix,
      });

      this.setAppState({ status: "showing_msg", msg: "mqtt_connecting" });

      mqtt_client.on_alive(() => {
        this.setAppState({
          status: "showing_msg",
          msg: "mqtt_connected_waiting_for_audioctl",
        });
      });

      mqtt_client.on_dead(() => {
        this.setAppState({ status: "showing_msg", msg: "mqtt_disconnected" });
      });

      const client = new SubClient(mqtt_client, {
        prefix: "audioctl/",
        online_topic: "audioctl/online",
      });

      client.on_alive(() => {
        this.setAppState({ status: "connected", client });
      });

      client.on_dead(() => {
        this.setAppState({ status: "showing_msg", msg: "audioctl_died" });
      });
    } else if (ws_url) {
      const client = new WSClient({ ws_url: ws_url });

      this.setAppState({ status: "showing_msg", msg: "ws_connecting" });

      client.on_alive(() => {
        this.setAppState({ status: "connected", client });
      });

      client.on_dead(() => {
        this.setAppState({ status: "showing_msg", msg: "ws_disconnected" });
      });
    } else {
      this.setAppState({ status: "showing_help" });
    }
  }

  render() {
    const state = this.state;
    switch (state.status) {
      case "connected":
        return <MixerUI client={state.client} />;
      case "showing_help":
        return help_message();
      case "showing_msg":
        return <section>{status_messages[state.msg]}</section>;
    }
  }
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
  );
}

function linkify(l: string): VNode {
  return <a href={l}>{l}</a>;
}

function ws_dflt_url() {
  return `${location.protocol}//${location.host}${location.pathname}?ws_url=http://localhost:8811/ws`;
}

function mqtt_dflt_url(): string {
  return `${location.protocol}//${location.host}${location.pathname}?mqtt_url=http://localhost:1880&mqtt_prefix=/fosdem/`;
}

function main() {
  setup_favicon();

  const mixer_el = document.querySelector<HTMLDivElement>("#mixer")!;
  render(<App />, mixer_el);
}

main();
