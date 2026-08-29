import type {ChannelState, SendState, BusState, Levels} from "./api_data.ts";
import {type Signal} from "@preact/signals";
import {VUSlider, Checkbox, VUMeter} from "./widgets.tsx";
import {EqCurve} from "./eqcurve.tsx";

type Props = {
  channel: ChannelState;
  buses: BusState[];
  idx: number;
  levels: Signal<Levels | null>;
  actions: InputActions;
};

export interface InputActions {
  set_gain: (gain: number) => Promise<void>;
  set_master_fader: (fader: number) => Promise<void>;
  set_master_unmuted: (unmuted: boolean) => Promise<void>;
  set_phantom: (on: boolean) => Promise<void>;
  send: (bus: number) => SendActions;
}

export interface SendActions {
  set_volume: (volume: number) => Promise<void>;
  set_unmuted: (unmuted: boolean) => Promise<void>;
  set_pre_fader: (pre: boolean) => Promise<void>;
  set_pre_mute: (pre: boolean) => Promise<void>;
}

export function MixerInput(props: Props) {
  const channel = props.channel;
  const actions = props.actions;

  return (
      <div className="channel">
        <button className="checkbox setup" command="show-modal" commandfor={"setup-panel-" + channel.name}>Setup
        </button>

        <dialog className="setup-panel" id={"setup-panel-" + channel.name} closedby="any">
          <div className="headerbar">
            <h4>Setup {channel.name}</h4>
            <button commandfor={"setup-panel-" + channel.name} command="close">&times;</button>
          </div>

          <main>
            <section>
              <header>Input configuration</header>

              <Checkbox
                  checked={channel.phantom}
                  className="phantom"
                  onNewUserVal={actions.set_phantom}
                  label={"+48V phantom"}
                  syncBackAfter={1300}
              />

              <span className="label">Input gain</span>
              <div className="sliders gain">
                <VUSlider
                    value={channel.gain}
                    min={-80}
                    max={60}
                    direction="vertical"
                    onNewUserVal={actions.set_gain}
                />
              </div>

            </section>
            <section>
              <header>Output routing</header>
              <div className="sends">
                {channel.sends.map((send, i) => (
                    <SendMap
                        key={i}
                        send={send}
                        bus={props.buses[i]}
                        actions={actions.send(i)}
                    />
                ))}
              </div>
            </section>
          </main>

        </dialog>
        <span className={["checkbox", "phantom", "indicator", channel.phantom ? "checked" : "unchecked"].join(" ")}>
          +48V
        </span>
        <button class="invisible" command="show-modal" commandfor={"eq-panel-" + channel.name}>
          <EqCurve bands={channel.eq} width={87} height={60}/>
        </button>
        <dialog className="eq-panel" id={"eq-panel-" + channel.name} closedby="any">
          <div className="headerbar">
            <h4>Equalizer for {channel.name}</h4>
            <button commandfor={"eq-panel-" + channel.name} command="close">&times;</button>
          </div>
          <main>
            <section>
              <EqCurve bands={channel.eq} width={600} height={250}/>
              <div class="bands">
                {channel.eq.map((band, i) => (
                    <div class="band">
                      <header>Band {i + 1}</header>
                      <select value={band.type}>
                        <option value={0}>Disabled</option>
                        <option value={1}>Low-pass</option>
                        <option value={2}>High-pass</option>
                        <option value={3}>Low-shelf</option>
                        <option value={4}>High-shelf</option>
                        <option value={5}>Notch</option>
                        <option value={6}>Peak</option>
                      </select>
                      <label>Frequency
                        <input type="number" value={band.frequency} min={19} max={2200}/>
                      </label>
                      <label>Gain
                        <input type="number" value={band.gain} min={-60} max={60} step={0.1}/>
                      </label>
                      <label>Q
                        <input type="number" value={band.q} defaultValue={0.717} step={0.01} />
                      </label>
                    </div>
                ))}
              </div>
            </section>
          </main>
        </dialog>
        <h3 title={channel.name} className="scribblestrip input">{channel.name}</h3>
        <div className="controls">
          <Checkbox
              className="mute inverted"
              checked={channel.master_unmuted}
              onNewUserVal={actions.set_master_unmuted}
              label="Mute"
              syncBackAfter={1300}
          />
          <div className="sliders master">
            <VUSlider
                value={channel.master_fader}
                min={-80}
                max={60}
                direction="vertical"
                onNewUserVal={actions.set_master_fader}
            />
            <VUMeter levels={props.levels} kind="inputs" idx={props.idx}/>
          </div>
        </div>
      </div>
  )
      ;
}

function SendMap(props: {
  send: SendState;
  bus: BusState;
  actions: SendActions;
}) {
  const {send, bus, actions} = props;
  return (
      <div className="send">
        <h4 title={bus.name} className="scribblestrip output">{bus.name}</h4>
        <VUSlider
            value={send.volume}
            min={-80}
            max={60}
            direction="vertical"
            onNewUserVal={actions.set_volume}
        />
        <Checkbox
            className="send-active"
            checked={send.unmuted}
            onNewUserVal={actions.set_unmuted}
            label="Active"
            syncBackAfter={1300}
        />
        <Checkbox
            checked={send.pre_channel_fader}
            onNewUserVal={actions.set_pre_fader}
            label="Pre Fader"
            syncBackAfter={1300}
        />
        <Checkbox
            checked={send.pre_channel_mute}
            onNewUserVal={actions.set_pre_mute}
            label="Pre Mute"
            syncBackAfter={1300}
        />
      </div>
  );
}
