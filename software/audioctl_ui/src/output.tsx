import type { BusState } from "./api_data.ts";
import { useId } from "preact/hooks";
import { Slider, Checkbox } from "./widgets.tsx";

type Props = {
  bus: BusState;
};

export function MixerOutput(props: Props) {
  const bus = props.bus;
  const id = useId();

  return (
    <div className="channel">
      <h3 title={bus.name}>{bus.label}</h3>
      <div className="controls">
        <div className="sliders master">
          <span className="label">master</span>
          <Slider value={bus.master_fader} />
        </div>
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          checked={bus.master_unmuted}
          label="unmuted"
        />
      </div>
    </div>
  );
}
