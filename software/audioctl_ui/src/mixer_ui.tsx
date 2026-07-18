import type {MisirkaClient} from "misirka"
import {MixerClient} from "./mixerclient.ts"
import type {Levels, MixerState} from "./api_data.ts"

import {element, ref} from "tsx-vanilla"
import {Input, Output} from "./components.tsx"
import {formatDb, logLinear} from "./helpers.ts"


export class MixerUI {
  constructor(mclient: MisirkaClient, private container: HTMLElement) {
    this.client = new MixerClient(mclient)
    mclient.on_alive(() => this.on_alive())
    mclient.on_dead(() => this.on_dead())
  }

  private client: MixerClient

  public toggleSetupMode(setup: boolean) {
    for(const gainSlider of document.querySelectorAll<HTMLDivElement>(`.inputs .gain`)) {
      gainSlider.querySelector<HTMLInputElement>(`input[type=range]`)!.disabled = !setup
      gainSlider.querySelector<HTMLDivElement>(`.volume`)!.style.visibility = setup ? 'visible' : 'hidden'
    }

    for(const phantom of document.querySelectorAll<HTMLInputElement>(`.inputs .phantom`)) {
      phantom.querySelector<HTMLInputElement>(`input[type=checkbox]`)!.disabled = !setup
      phantom.style.visibility = setup ? 'visible' : 'hidden'
    }
  }


  private async on_alive() {
    const initialState = await this.client.get_state()

    const inputsContainer = ref<HTMLDivElement>()
    const outputsContainer = ref<HTMLDivElement>()
    const setupModeSelector = ref<HTMLInputElement>()

    this.container.replaceChildren(
      <section>
        <div>
          <label htmlFor="setup-mode">Setup mode</label>
          <input type="checkbox" id="setup-mode" ref={setupModeSelector}/>
        </div>

        <div className='mixer'>
          <div className='mixer'>
            <div className='inputs channellist' ref={inputsContainer}/>
          </div>
          <div className='mixer'>
            <div className='outputs channellist' ref={outputsContainer}/>
          </div>
        </div>
      </section>,
    )

    this.setupInputs(inputsContainer.value!, initialState)
    this.setupOutputs(outputsContainer.value!, initialState)


    setupModeSelector.value!.onchange = _ => this.toggleSetupMode(setupModeSelector.value!.checked)
  this.toggleSetupMode(setupModeSelector.value!.checked)

    await this.client.subscribe_state(s => this.updateState(s))
    await this.client.subscribe_levels(l => this.updateVu(initialState, l))
  }

  private async on_dead() {
  }

  private async updateVu(state: MixerState, levels: Levels) {
    for (const [i, db] of levels.smooth.inputs.entries()) {
      const input = state.channels[i]
      const channel = this.container.querySelector<HTMLDivElement>(`.inputs [data-name=${input.name}]`)!

      channel.querySelector<HTMLMeterElement>(`meter`)!.value = logLinear(db)
      channel.querySelector<HTMLDivElement>(`.meterdb`)!.innerText = formatDb(db)

    }
    for (const [i, db] of levels.smooth.buses.entries()) {
      const output = state.buses[i]
      const bus =  this.container.querySelector<HTMLDivElement>(`.outputs [data-name=${output.name}]`)!
      bus.querySelector<HTMLMeterElement>(`meter`)!.value = logLinear(db)
      bus.querySelector<HTMLDivElement>(`.meterdb`)!.innerText = formatDb(db)
    }
  }

  private async updateState(state: MixerState) {
    for (const channel of state.channels) {
      const input = this.container.querySelector<HTMLDivElement>(`.inputs [data-name=${channel.name}]`)!
      input.querySelector<HTMLInputElement>(`input[type=range]`)!.value = channel.gain.toString()
      input.querySelector<HTMLDivElement>(`.gaindb`)!.innerText = formatDb(channel.gain)

      input.querySelector<HTMLInputElement>(`.phantom input[type=checkbox]`)!.checked = channel.phantom
    }

    for (const bus of state.buses) {
      const output =  this.container.querySelector<HTMLDivElement>(`.outputs [data-name=${bus.name}]`)!
      output.querySelector<HTMLInputElement>(`input[type=range]`)!.value = bus.volume.toString()
      output.querySelector<HTMLDivElement>(`.gaindb`)!.innerText = formatDb(bus.volume)
    }

    for(const ch of state.channels) {
      for(const [j, bus] of state.buses.entries()) {
        const send = this.container.querySelector<HTMLDivElement>(`.inputs [data-from=${ch.name}][data-to=${bus.name}]`)!

        send.querySelector<HTMLInputElement>(`input[type=range]`)!.value = ch.sends[j].volume.toString()
        send.querySelector<HTMLInputElement>(`.gaindb`)!.innerText = formatDb(ch.sends[j].volume)
        send.querySelector<HTMLInputElement>(`input.mute[type=checkbox]`)!.checked = ch.sends[j].unmuted
      }
    }
  }

  private setupOutputs(container: HTMLDivElement, state: MixerState) {
    container.replaceChildren(
      ...state.buses.entries().map(([i, out]) =>
        <Output output={out} onVolume={volume => this.client.set_bus_volume(i, volume)}/>))
  }

  private setupInputs(container: HTMLDivElement, state: MixerState) {
    container.replaceChildren(
      ...state.channels.entries().map(([i, inp]) =>
        <Input input={inp} state={state}
          onGain={gain => this.client.set_in_gain(i, gain)}
          onSendMute={bus => muted => this.client.set_matrix_send(i, bus, !muted)}
          onSendVolume={bus => volume => this.client.set_matrix_volume(i, bus, volume)}
          onPhantom={enabled => this.client.set_phantom(i, enabled)}
        />))
  }
}
