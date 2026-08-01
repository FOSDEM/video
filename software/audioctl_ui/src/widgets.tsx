import { formatDb } from "./helpers.ts";

export function Slider(props: { value: number }) {
  return (
    <div className="volume">
      <input type="range" min="-80" max="60" step="0.6" value={props.value} />
      <div className="db gaindb">{formatDb(props.value)}</div>
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
