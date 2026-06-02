import type { MisirkaClient } from "misirka"
import { MixerStateSchema, LevelsSchema, OkRespSchema } from "./api_data.ts"
import type { MixerState, Levels } from "./api_data.ts"

export class MixerClient {
  constructor(private client: MisirkaClient) {
  }

  async get_state(): Promise<MixerState> {
    return await this.client.get('state', MixerStateSchema)
  }

  async set_in_gain(chan_idx: number, gain: number) {
    await this.call_expecting_ok('set-in-gain', {
      channel: chan_idx,
      gain: gain,
    })
  }

  async set_phantom(chan_idx: number, phantom: boolean) {
    await this.call_expecting_ok('set-phantom', {
      channel: chan_idx,
      phantom: phantom,
    })
  }

  async set_matrix_send(chan_idx: number, bus_idx: number, unmuted: boolean) {
    await this.call_expecting_ok('set-matrix-send', {
      channel: chan_idx,
      bus: bus_idx,
      unmuted: unmuted,
    })
  }

  async set_matrix_volume(chan_idx: number, bus_idx: number, volume: number) {
    await this.call_expecting_ok('set-matrix-volume', {
      channel: chan_idx,
      bus: bus_idx,
      volume: volume,
    })
  }

  async set_bus_volume(bus_idx: number, volume: number) {
    await this.call_expecting_ok('set-bus-volume', {
      bus: bus_idx,
      volume: volume,
    })
  }

  async subscribe_state(handler: (state: MixerState) => void) {
    await this.client.subscribe(
      'state', MixerStateSchema, handler,
    )
  }

  async subscribe_levels(handler: (levels: Levels) => void) {
    await this.client.subscribe(
      'levels', LevelsSchema, handler,
    )
  }

  private async call_expecting_ok(method: string, param: any) {
    await this.client.call(method, param, OkRespSchema)
  }
}
