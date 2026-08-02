import { Component } from "preact";
import { useComputed, type Signal } from "@preact/signals";
import type { QUI } from "rtui";
import type { Levels } from "./api_data.ts";
import { formatDb, logLinear } from "./helpers.ts";

type RTProps<T> = {
  id: string;
  value: T;
  onInput?: (value: T) => void | Promise<void>;
  qui?: QUI;
  reset_after?: number;
};

type RTState<T> = {
  user_req: T;
};

// base for widgets that show both a "real" value that comes from
// the backend and an "user" value that is set by the user.
// upon failure to set the value, or upon optional timeout,
// the user value resets to the real value.
abstract class RTWidget<T, P extends RTProps<T>> extends Component<
  P,
  RTState<T>
> {
  state: RTState<T> = { user_req: this.props.value };

  componentDidUpdate(prev_props: P) {
    if (prev_props.value !== this.props.value) {
      this.setState({ user_req: this.props.value });
    }
  }

  // reset the user req value to the backend-provided one,
  // since it failed to update
  protected reset = () => {
    this.setState({ user_req: this.props.value });
  };

  protected exec_action(action: () => void | Promise<void>) {
    const { qui, id, reset_after } = this.props;
    if (qui) {
      qui.add({
        handler: async () => {
          await action();
        },
        bucket: id,
        reset_after,
        reset: this.reset,
      });
    } else {
      action();
    }
  }
}

type SliderProps = RTProps<number> & {
  min: number;
  max: number;
  direction: "vertical" | "horizontal";
  extra_indicators?: [string, number][];  // list of tuples key -> coef to display in addition to 'real' and 'user'
};

export class Slider extends RTWidget<number, SliderProps> {
  private pct(value: number): number {
    const { min, max } = this.props;
    const frac = (value - min) / (max - min);
    return Math.min(100, Math.max(0, frac * 100));
  }

  private bar_style(value: number) {
    const size = `${this.pct(value)}%`;
    if (this.props.direction == "vertical") {
      return { height: size };
    } else {
      return { width: size };
    }
  }

  private dragging = false;

  private handle_pointer_commit(e: PointerEvent) {
    const { onInput, min, max, direction } = this.props;
    if (!onInput) return;
    const rect = (e.currentTarget as HTMLElement).getBoundingClientRect();
    const frac =
      direction === "vertical"
        ? 1 - (e.clientY - rect.top) / rect.height
        : (e.clientX - rect.left) / rect.width;
    const clamped = Math.min(1, Math.max(0, frac));
    const value = min + clamped * (max - min);
    this.setState({ user_req: value });
    this.exec_action(() => onInput(value));
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
    const { id, value, direction, extra_indicators } = this.props;
    const { user_req } = this.state;
    return (
      <div
        className={`slider ${direction}`}
        id={id}
        onPointerDown={this.handle_pointer_down}
        onPointerMove={this.handle_pointer_move}
        onPointerUp={this.handle_pointer_up}
      >
        <div className="real" style={this.bar_style(value)} />
        <div className="user" style={this.bar_style(user_req)} />
        {extra_indicators?.map(([cls, v]) => (
          <div key={cls} className={cls} style={this.bar_style(v)} />
        ))}
      </div>
    );
  }
}

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

type CheckboxProps = RTProps<boolean> & {
  label: string;
  className?: string;
};

export class Checkbox extends RTWidget<boolean, CheckboxProps> {
  private handle_click = () => {
    const { onInput } = this.props;
    if (!onInput) return;
    const requested = !this.state.user_req;
    this.setState({ user_req: requested });
    this.exec_action(() => onInput(requested));
  };

  render() {
    const { id, value, label, className } = this.props;
    const { user_req } = this.state;

    let state: string;
    if (user_req === value) {
      state = value ? "checked" : "unchecked";
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
