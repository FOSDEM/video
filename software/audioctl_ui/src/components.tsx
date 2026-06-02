import type {BusState, ChannelState, MixerState} from "./api_data"
import {element} from "tsx-vanilla"

export const VolumeSlider = (props: {onVolume: (volume: number) => void})=>
  <div className='volume'>
    <input type='range' min='-80' max='60' step='0.6'
      oninput={e => props.onVolume(+e.target.value)}/>
    <div className='db gaindb'/>
  </div> as HTMLDivElement

export const Output = (props: {output: BusState, onVolume: (volume: number) => void}) =>
    <div className='channel' dataset={{name: props.output.name}}>
      <h3 title={props.output.name}>{props.output.label}</h3>
      <div className='sliders'>
        <VolumeSlider onVolume={props.onVolume}/>
        <Meter/>
      </div>
    </div> as HTMLDivElement

export const Meter = () =>
    <div className='vu-meter'>
      <meter min={0} max={1} low={0.5} optimum={0.2} high={0.8}/>
      <div className='db meterdb'/>
    </div> as HTMLDivElement

export const Send = (props: {input: ChannelState, output: BusState, onMute: (muted: boolean) => void, onVolume: (volume: number) => void}) =>
    <div className='send' dataset={{from: props.input.name, to: props.output.name}}>
      <h4 title={props.output.name}>{props.output.label}</h4>
      <VolumeSlider onVolume={props.onVolume} />
      <input type='checkbox' className='mute'
        oninput={e => props.onMute(!e.target.checked)}/>
    </div> as HTMLDivElement

export const Input = (props: {
    input: ChannelState,
    state: MixerState,
    onGain: (gain: number) => void,
    onPhantom: (enabled: boolean) => void,
    onSendVolume: (id: number) => (volume: number) => void,
    onSendMute: (id: number) => (muted: boolean) => void,
}) => {
  const sends = props.input.sends.entries().map(([i, _]) =>
    <Send input={props.input} output={props.state.buses[i]} 
      onMute={props.onSendMute(i)} onVolume={props.onSendVolume(i)}
    />)

  return <div className='channel' dataset={{name: props.input.name}}>
    <h3 title={props.input.name}>{props.input.label}</h3>
    <div className='controls'>
      <div className='sliders gain'>
        <VolumeSlider onVolume={props.onGain}/>
        <Meter/>
      </div>
      <div className='phantom' title='Phantom power'>
        <label htmlFor={`phantom-${props.input.name}`}>&#x1F47B; </label>
        <input type='checkbox'
          id={`phantom-${props.input.name}`}
          onchange={e => props.onPhantom(e.target.checked)}/>
      </div>
    </div>
    <div className='sends' children={sends.toArray()}/>
  </div> as HTMLDivElement
}
