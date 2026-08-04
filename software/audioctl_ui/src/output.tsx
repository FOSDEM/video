import type { BusState, Levels } from "./api_data.ts";
import type { Signal } from "@preact/signals";
import { VUSlider, Checkbox, VUMeter } from "./widgets.tsx";

type Props = {
  bus: BusState;
  idx: number;
  levels: Signal<Levels | null>;
  actions: OutputActions;
};

export type OutputActions = {
  set_master_fader: (fader: number) => Promise<void>;
  set_master_unmuted: (unmuted: boolean) => Promise<void>;
};

export function MixerOutput(props: Props) {
  const bus = props.bus;
  const actions = props.actions;

  return (
    <div className="channel">
      <h3 title={bus.name}>{bus.label}</h3>
      <div className="controls">
        <div className="sliders master">
          <VUSlider
            value={bus.master_fader}
            min={-80}
            max={60}
            direction="vertical"
            onNewUserVal={actions.set_master_fader}
            syncBackAfter={1300}
          />
          <VUMeter levels={props.levels} kind="buses" idx={props.idx} />
        </div>
        <Checkbox
          className="mute"
          checked={bus.master_unmuted}
          onNewUserVal={actions.set_master_unmuted}
          label="unmuted"
          syncBackAfter={1300}
        />
      </div>
    </div>
  );
}
