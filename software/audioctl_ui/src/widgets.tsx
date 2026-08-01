import { useComputed, type Signal } from "@preact/signals";
import type { Levels } from "./api_data.ts";
import { formatDb, logLinear } from "./helpers.ts";

export function Slider(props: {
  value: number;
  onInput?: (value: number) => void;
}) {
  const { onInput } = props;
  return (
    <div className="volume">
      <input
        type="range"
        min="-80"
        max="60"
        step="0.6"
        value={props.value}
        onInput={onInput && ((e) => onInput(+e.currentTarget.value))}
      />
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

export function Checkbox(props: {
  id: string;
  checked: boolean;
  label: string;
  className?: string;
  onInput?: (checked: boolean) => void;
}) {
  const { onInput } = props;
  return (
    <span>
      <input
        id={props.id}
        type="checkbox"
        className={props.className}
        checked={props.checked}
        onInput={onInput && ((e) => onInput(e.currentTarget.checked))}
      />
      <label for={props.id}>{props.label}</label>
    </span>
  );
}
