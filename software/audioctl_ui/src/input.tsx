import type { ChannelState, SendState } from "./api_data.ts";
import { useId } from "preact/hooks";
import { formatDb } from "./helpers.ts";

type Props = {
  channel: ChannelState;
};

function Slider(props: { value: number }) {
  return (
    <div className="volume">
      <input type="range" min="-80" max="60" step="0.6" value={props.value} />
      <div className="db gaindb">{formatDb(props.value)}</div>
    </div>
  );
}

function Checkbox(props: {
  id: string;
  checked: boolean;
  label: string;
  className?: string;
}) {
  return (
    <span>
      <input
        id={props.id}
        type="checkbox"
        className={props.className}
        checked={props.checked}
      />
      <label for={props.id}>{props.label}</label>
    </span>
  );
}

function SendMap(props: { id: string; send: SendState; i: number }) {
  const { id, send, i } = props;
  return (
    <div className="send">
      <h4>bus {i}</h4>
      <Slider value={send.volume} />
      <Checkbox
        id={`${id}-${i}-unmuted`}
        className="mute"
        checked={send.unmuted}
        label="unmuted"
      />
      <Checkbox
        id={`${id}-${i}-pre-fader`}
        checked={send.pre_channel_fader}
        label="pre-fader"
      />
      <Checkbox
        id={`${id}-${i}-pre-mute`}
        checked={send.pre_channel_mute}
        label="pre-mute"
      />
    </div>
  );
}

export function MixerInput(props: Props) {
  const channel = props.channel;
  const id = useId();

  return (
    <div className="channel">
      <h3 title={channel.name}>{channel.label}</h3>
      <div className="controls">
        <div className="sliders gain">
          <span className="label">gain</span>
          <Slider value={channel.gain} />
        </div>
        <div className="sliders master">
          <span className="label">master</span>
          <Slider value={channel.master_fader} />
        </div>
        <div className="phantom" title="Phantom power">
          <Checkbox
            id={`${id}-phantom`}
            checked={channel.phantom}
            label={"\u{1F47B}"}
          />
          <Checkbox
            id={`${id}-master-unmuted`}
            className="mute"
            checked={channel.master_unmuted}
            label="unmuted"
          />
        </div>
      </div>
      <div className="sends">
        {channel.sends.map((send, i) => (
          <SendMap key={i} id={`${id}-send`} send={send} i={i} />
        ))}
      </div>
    </div>
  );
}
