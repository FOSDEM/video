import { z } from "zod"

const SendStateSchema = z.object({
  unmuted: z.boolean(),
  volume: z.number(),
})

const ChannelStateSchema = z.object({
  name: z.string(),
  label: z.string(),
  gain: z.number(),
  phantom: z.boolean(),
  sends: z.array(SendStateSchema),
})

const BusStateSchema = z.object({
  name: z.string(),
  label: z.string(),
  volume: z.number(),
})

const MixerStateSchema = z.object({
  channels: z.array(ChannelStateSchema),
  buses: z.array(BusStateSchema),
})

const LevelsBlockSchema = z.object({
  inputs: z.array(z.number()),
  buses: z.array(z.number()),
})

const LevelsSchema = z.object({
  rms: LevelsBlockSchema,
  peak: LevelsBlockSchema,
  smooth: LevelsBlockSchema,
})

const OkRespSchema = z.literal("ok")

export type SendState = z.infer<typeof SendStateSchema>;
export type ChannelState = z.infer<typeof ChannelStateSchema>;
export type BusState = z.infer<typeof BusStateSchema>;
export type MixerState = z.infer<typeof MixerStateSchema>;
export type LevelsBlock = z.infer<typeof LevelsBlockSchema>;
export type Levels = z.infer<typeof LevelsSchema>;
export type OkResp = z.infer<typeof OkRespSchema>;

export {
  SendStateSchema,
  ChannelStateSchema,
  BusStateSchema,
  MixerStateSchema,
  LevelsBlockSchema,
  LevelsSchema,
  OkRespSchema,
}
