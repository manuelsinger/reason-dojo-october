open Util;

module Img = {
  type t;
  [@bs.new] external make: unit => t = "Image";

  [@bs.set] external onload: (t, unit => unit) => unit = "";
  [@bs.set] external src: (t, string) => unit = "";
};

type state = {loaded: bool};
type action =
  | Loaded;

let component = ReasonReact.reducerComponent("Prefetch");

[@genType]
let make = (~images: array(string), children) => {
  ...component,
  initialState: () => {loaded: false},

  didMount: self =>
    images
    |> Array.map(url => {
         let img = Img.make();
         Js.Promise.make((~resolve, ~reject as _) => {
           img->Img.onload(() => resolve(. url));
           img->Img.src(url);
         });
       })
    |> Js.Promise.all
    |> Js.Promise.then_(_ => {
         self.send(Loaded);
         Js.Promise.resolve();
       })
    |> ignore,

  reducer: (action, _state) =>
    switch (action) {
    | Loaded => ReasonReact.Update({loaded: true})
    },

  render: self =>
    switch (self.state.loaded) {
    | false => <div> {s("Processing...")} </div>
    | true => children
    },
};