import { useComputed, type Signal } from "@preact/signals";
import type { Levels } from "./api_data.ts";
import { formatDb, logLinear } from "./helpers.tsx";
import { Slider } from "rtui/preact";
import type { SliderProps } from "rtui/preact";
export { Checkbox } from "rtui/preact";

export function VUSlider(props: SliderProps) {
  return (
    <div className="vu-slider">
      <Slider {...props} />
      <div className="db gaindb">{formatDb(props.value)}</div>
    </div>
  );
}

export function VUMeter(props: {
  levels: Signal<Levels | null>;
  kind: "inputs" | "buses";
  idx: number;
}) {
  const { levels, kind, idx } = props;
  const db = useComputed(() => levels.value?.smooth[kind][idx] ?? -120);
  const value = useComputed(() => logLinear(db.value));
  const label = useComputed(() => formatDb(db.value));

  return (
    <div className="vu-meter">
      <meter min={0} max={1} low={0.5} optimum={0.2} high={0.8} value={value} />
      <div className="db meterdb">{label}</div>
    </div>
  );
}
