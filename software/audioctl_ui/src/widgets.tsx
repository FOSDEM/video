import { Component } from "preact";
import { useComputed, type Signal } from "@preact/signals";
import type { Levels } from "./api_data.ts";
import { formatDb, logLinear } from "./helpers.ts";

type SliderProps = {
  value: number;
  min: number;
  max: number;
  onInput?: (value: number) => void;
};

type SliderState = {
  user_req: number;
};

export class Slider extends Component<SliderProps, SliderState> {
  state: SliderState = { user_req: this.props.value };

  componentDidUpdate(prev_props: SliderProps) {
    if (prev_props.value !== this.props.value) {
      this.setState({ user_req: this.props.value });
    }
  }

  private pct(value: number): number {
    const { min, max } = this.props;
    const frac = (value - min) / (max - min);
    return Math.min(100, Math.max(0, frac * 100));
  }

  private dragging = false;

  private handle_pointer_commit(e: PointerEvent) {
    const { onInput, min, max } = this.props;
    if (!onInput) return;
    const rect = (e.currentTarget as HTMLElement).getBoundingClientRect();
    const frac = 1 - (e.clientY - rect.top) / rect.height;
    const clamped = Math.min(1, Math.max(0, frac));
    const value = min + clamped * (max - min);
    this.setState({ user_req: value });
    onInput(value);
  }

  private handle_pointer_down = (e: PointerEvent) => {
    this.dragging = true;
    (e.currentTarget as HTMLElement).setPointerCapture(e.pointerId);
    this.handle_pointer_commit(e);
  };

  private handle_pointer_move = (e: PointerEvent) => {
    if (this.dragging) this.handle_pointer_commit(e);
  };

  private handle_pointer_up = (e: PointerEvent) => {
    this.dragging = false;
    (e.currentTarget as HTMLElement).releasePointerCapture(e.pointerId);
  };

  render() {
    const { value } = this.props;
    const { user_req } = this.state;
    return (
      <div className="slider">
        <div
          className="container"
          onPointerDown={this.handle_pointer_down}
          onPointerMove={this.handle_pointer_move}
          onPointerUp={this.handle_pointer_up}
        >
          <div className="real" style={{ height: `${this.pct(value)}%` }} />
          <div className="user" style={{ height: `${this.pct(user_req)}%` }} />
        </div>
        <div className="db gaindb">{formatDb(value)}</div>
      </div>
    );
  }
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
