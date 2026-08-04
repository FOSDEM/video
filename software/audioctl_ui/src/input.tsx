import type { ChannelState, SendState, BusState, Levels } from "./api_data.ts";
import { type Signal, useSignal } from "@preact/signals";
import { VUSlider, Checkbox, VUMeter } from "./widgets.tsx";

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
  const setup = useSignal(false);

  return (
    <div className="channel">
      <h3 title={channel.name}>{channel.label}</h3>
      <div className="controls">
        <Checkbox
          checked={setup.value}
          label={"\u{1F527}"}
          onNewUserVal={async (val) => {
            setup.value = val;
          }}
        />
        {setup.value && (
          <div className="sliders gain">
            <span className="label">gain</span>
            <VUSlider
              value={channel.gain}
              min={-80}
              max={60}
              direction="vertical"
              onNewUserVal={actions.set_gain}
              syncBackAfter={1300}
            />
          </div>
        )}
        <div className="sliders master">
          <VUSlider
            value={channel.master_fader}
            min={-80}
            max={60}
            direction="vertical"
            onNewUserVal={actions.set_master_fader}
            syncBackAfter={1300}
          />
          <VUMeter levels={props.levels} kind="inputs" idx={props.idx} />
        </div>
        {setup.value && (
          <Checkbox
            checked={channel.phantom}
            onNewUserVal={actions.set_phantom}
            label={"\u{1F47B}"}
            syncBackAfter={1300}
          />
        )}
        <Checkbox
          className="mute"
          checked={channel.master_unmuted}
          onNewUserVal={actions.set_master_unmuted}
          label="unmuted"
          syncBackAfter={1300}
        />
      </div>
      {setup.value && (
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
      )}
    </div>
  );
}

function SendMap(props: {
  send: SendState;
  bus: BusState;
  actions: SendActions;
}) {
  const { send, bus, actions } = props;
  return (
    <div className="send">
      <h4 title={bus.name}>{bus.label}</h4>
      <VUSlider
        value={send.volume}
        min={-80}
        max={60}
        direction="vertical"
        onNewUserVal={actions.set_volume}
        syncBackAfter={1300}
      />
      <Checkbox
        className="mute"
        checked={send.unmuted}
        onNewUserVal={actions.set_unmuted}
        label="unmuted"
        syncBackAfter={1300}
      />
      <Checkbox
        checked={send.pre_channel_fader}
        onNewUserVal={actions.set_pre_fader}
        label="pre-fader"
        syncBackAfter={1300}
      />
      <Checkbox
        checked={send.pre_channel_mute}
        onNewUserVal={actions.set_pre_mute}
        label="pre-mute"
        syncBackAfter={1300}
      />
    </div>
  );
}
