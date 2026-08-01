import { Component } from "preact";
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

type CheckboxProps = {
  id: string;
  checked: boolean;
  label: string;
  className?: string;
  onInput?: (checked: boolean) => void;
};

type CheckboxState = {
  user_req: boolean;
};

export class Checkbox extends Component<CheckboxProps, CheckboxState> {
  state: CheckboxState = { user_req: this.props.checked };

  componentDidUpdate(prev_props: CheckboxProps) {
    if (prev_props.checked !== this.props.checked) {
      this.setState({ user_req: this.props.checked });
    }
  }

  private handle_click = () => {
    const { onInput } = this.props;
    if (!onInput) return;
    const requested = !this.state.user_req;
    this.setState({ user_req: requested });
    onInput(requested);
  };

  render() {
    const { id, checked, label, className } = this.props;
    const { user_req } = this.state;

    let state: string;
    if (user_req === checked) {
      state = checked ? "checked" : "unchecked";
    } else {
      state = user_req ? "wants-check" : "wants-uncheck";
    }

    return (
      <span
        id={id}
        className={["checkbox", state, className].filter(Boolean).join(" ")}
        onClick={this.handle_click}
      >
        {label}
      </span>
    );
  }
}
