export function formatDb(n: number): string {
  const sign = n < 0 ? "-" : "\u00A0";
  const abs = Math.abs(n);
  const intPart = String(Math.floor(abs)).padStart(3, "0");
  const decPart = Math.floor((abs % 1) * 10);
  return `${sign}${intPart}.${decPart} dB`;
}

export function logLinear(db: number): number {
  return 0.79306 * Math.exp(db * 0.0527087);
}

export function linkify(l: string) {
  return <a href={l}>{l}</a>;
}

export function ws_dflt_url() {
  return `${location.protocol}//${location.host}${location.pathname}?ws_url=http://localhost:8811/ws`;
}

export function mqtt_dflt_url(): string {
  return `${location.protocol}//${location.host}${location.pathname}?mqtt_url=http://localhost:1880&mqtt_prefix=/fosdem/`;
}
