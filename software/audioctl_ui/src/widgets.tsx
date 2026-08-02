import { useComputed, type Signal } from "@preact/signals"
import type { Levels } from "./api_data.ts"
import { formatDb, logLinear } from "./helpers.tsx"
import { useEffect, useState} from "preact/hooks"
import useReactive from "./reactive.ts"

type SliderProps = {
  id: string;
  value: number;
  min: number;
  max: number;
  onInput?: (value: number) => void | Promise<void>;
  reset_after?: number;
  direction: "vertical" | "horizontal";
  extra_indicators?: [string, number][];  // list of tuples key -> coef to display in addition to 'real' and 'user'
};

export function VUSlider(props: SliderProps) {
  return (
    <div className="vu-slider">
      <Slider {...props} />
      <div className="db gaindb">{formatDb(props.value)}</div>
    </div>
  )
}

export function Slider(props: SliderProps) {
  const [dragging, setDragging] = useState(false)

  const [req, setReq] = useReactive(props.id,
    props.value,
    props.onInput ?? (() => null),
    props.reset_after,
  )

  const pct = (value: number) => {
    const { min, max } = props
    const frac = (value - min) / (max - min)
    return Math.min(100, Math.max(0, frac * 100))
  }

  const bar_style = (value: number) => {
    const size = `${pct(value)}%`
    if (props.direction == "vertical") {
      return { height: size }
    } else {
      return { width: size }
    }
  }

  const handle_pointer_commit = (e: PointerEvent) => {
    const { min, max, direction } = props
    const rect = (e.currentTarget as HTMLElement).getBoundingClientRect()
    const frac =
        direction === "vertical"
          ? 1 - (e.clientY - rect.top) / rect.height
          : (e.clientX - rect.left) / rect.width

    const clamped = Math.min(1, Math.max(0, frac))
    const value = min + clamped * (max - min)
    setReq(value)
  }

  const handle_pointer_down = (e: PointerEvent) => {
    setDragging(true);
    (e.currentTarget as HTMLElement).setPointerCapture(e.pointerId)
    handle_pointer_commit(e)
  }

  const handle_pointer_move = (e: PointerEvent) => {
    if (dragging) handle_pointer_commit(e)
  }

  const handle_pointer_up = (e: PointerEvent) => {
    setDragging(false);
    (e.currentTarget as HTMLElement).releasePointerCapture(e.pointerId)
  }

  const { id, extra_indicators, value, direction } = props

  return (
    <div
      className={`slider ${direction}`}
      id={id}
      onPointerDown={handle_pointer_down}
      onPointerMove={handle_pointer_move}
      onPointerUp={handle_pointer_up}
    >
      <div className="real" style={bar_style(value)} />
      <div className="user" style={bar_style(req)} />
      {extra_indicators?.map(([cls, v]) => (
        <div key={cls} className={cls} style={bar_style(v)} />
      ))}
    </div>
  )
}

export function VUMeter(props: {
  levels: Signal<Levels | null>;
  kind: "inputs" | "buses";
  idx: number;
}) {
  const { levels, kind, idx } = props
  const db = useComputed(() => levels.value?.smooth[kind][idx] ?? -120)
  const value = useComputed(() => logLinear(db.value))
  const label = useComputed(() => formatDb(db.value))

  return (
    <div className="vu-meter">
      <meter min={0} max={1} low={0.5} optimum={0.2} high={0.8} value={value} />
      <div className="db meterdb">{label}</div>
    </div>
  )
}

type CheckboxProps = {
  id: string;
  checked: boolean;
  label: string;
  className?: string;
  onInput?: (checked: boolean) => any;
  reset_after?: number;
};


export function Checkbox(props: CheckboxProps) {
  const [bc, setBc] = useState('')
  const [req, setReq] = useReactive(props.id,
    props.checked,
    props.onInput ?? (_ => null),
    props.reset_after,
  )

  useEffect(() => {
    if (req === props.checked) {
      setBc(req ? "checked" : "unchecked")
    } else {
      setBc(req ? "wants-check" : "wants-uncheck")
    }}, [req, props.checked])

  return (
    <span
      id={props.id}
      className={["checkbox", bc, props.className].filter(Boolean).join(" ")}
      onClick={() => setReq(!req)}
    >
      {props.label}
    </span>
  )
}