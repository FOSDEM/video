import type { BusState } from "./api_data.ts";
import { useId } from "preact/hooks";
import { Slider, Checkbox } from "./widgets.tsx";

type Props = {
  bus: BusState;
  actions: OutputActions;
};

export type OutputActions = {
  set_master_fader: (fader: number) => void;
  set_master_unmuted: (unmuted: boolean) => void;
};

export function MixerOutput(props: Props) {
  const bus = props.bus;
  const actions = props.actions;
  const id = useId();

  return (
    <div className="channel">
      <h3 title={bus.name}>{bus.label}</h3>
      <div className="controls">
        <div className="sliders master">
          <span className="label">master</span>
          <Slider value={bus.master_fader} onInput={actions.set_master_fader} />
        </div>
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          checked={bus.master_unmuted}
          onInput={actions.set_master_unmuted}
          label="unmuted"
        />
      </div>
    </div>
  );
}
