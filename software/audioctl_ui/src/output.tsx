import type { BusState, Levels } from "./api_data.ts";
import { useId } from "preact/hooks";
import type { Signal } from "@preact/signals";
import type { QUI } from "rtui";
import { VUSlider, Checkbox, VUMeter } from "./widgets.tsx";

type Props = {
  bus: BusState;
  idx: number;
  levels: Signal<Levels | null>;
  actions: OutputActions;
  qui: QUI;
};

export type OutputActions = {
  set_master_fader: (fader: number) => Promise<void>;
  set_master_unmuted: (unmuted: boolean) => Promise<void>;
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
          <VUSlider
            id={`${id}-master-fader`}
            value={bus.master_fader}
            min={-80}
            max={60}
            direction="vertical"
            onInput={actions.set_master_fader}
            qui={props.qui}
            reset_after={1300}
          />
          <VUMeter levels={props.levels} kind="buses" idx={props.idx} />
        </div>
        <Checkbox
          id={`${id}-master-unmuted`}
          className="mute"
          value={bus.master_unmuted}
          onInput={actions.set_master_unmuted}
          label="unmuted"
          qui={props.qui}
          reset_after={1300}
        />
      </div>
    </div>
  );
}
