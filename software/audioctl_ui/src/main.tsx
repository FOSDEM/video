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

type AppState = {
  status: VNode | null;
  connected_client: MisirkaClient | null;
};

class App extends Component<object, AppState> {
  state: AppState = { status: <span>initialising</span>, connected_client: null };

  componentDidMount() {
    const client = this.connect();

    if (client) {
      client.on_alive(() => {
        this.setState({ connected_client: client, status: null });
      });
      client.on_dead(() => {
        this.setState({ connected_client: null, status: null });
      });
    }
  }

  private setStatus = (status: VNode) => {
    this.setState({ status });
  };

  private textStatus = (text: string) => {
    this.setStatus(<span>{text}</span>);
  };

  private connect(): MisirkaClient | null {
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

      this.textStatus("connecting to MQTT");

      mqtt_client.on_alive(() => {
        this.textStatus("MQTT connected, waiting for audioctl to appear");
      });

      mqtt_client.on_dead(() => {
        this.textStatus("MQTT disconnected, reconnecting");
      });

      const mclient = new SubClient(mqtt_client, {
        prefix: "audioctl/",
        online_topic: "audioctl/online",
      });

      mclient.on_dead(() => {
        this.textStatus(
          "MQTT still connected but audioctl died, waiting for it to appear",
        );
      });

      return mclient;
    } else if (ws_url) {
      const mclient = new WSClient({ ws_url: ws_url });

      this.textStatus("connecting to websocket");

      mclient.on_dead(() => {
        this.textStatus("websocket disconnected, reconnecting");
      });

      return mclient;
    } else {
      this.setStatus(help_message());
      return null;
    }
  }

  render() {
    return (
      <>
        {this.state.status && (
          <section>{this.state.status}</section>
        )}
        {this.state.connected_client && (
          <MixerUI client={this.state.connected_client} />
        )}
      </>
    );
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
