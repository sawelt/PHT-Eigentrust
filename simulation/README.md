# Simulation framework for p2p PHT reputation network
Folder structure:
- lib: contains basic framework classes and interfaces
- basic_impl: implements necessary component for the interfaces
## TODO:
- implement baseline to compare to! <img src="https://cdn3.emoji.gg/emojis/9109_Sad_Cat_Thumbs_Up.png" width="30">
- implement station rating train users <img src="https://ih0.redbubble.net/image.551616059.6925/flat,1000x1000,075,f.u4.jpg" width=30>
- json output <img src="https://cdn3.emoji.gg/emojis/9109_Sad_Cat_Thumbs_Up.png" width="30">
- implement that if two stations have the same trust value, they are chosen equally likely <img src="https://cdn3.emoji.gg/emojis/9109_Sad_Cat_Thumbs_Up.png" width="30">
- multiplex <img src="https://cdn3.emoji.gg/emojis/9109_Sad_Cat_Thumbs_Up.png" width="30">
- personalized <img src="https://ih0.redbubble.net/image.551616059.6925/flat,1000x1000,075,f.u4.jpg" width=30>
- replan selection strategy, random or price based <img src="https://cdn3.emoji.gg/emojis/9109_Sad_Cat_Thumbs_Up.png" width="30">
- first execution w/o ran CalculationStrategy -> trust values do not consider pre-trusted?
- train user as train creator <img src="https://cdn3.emoji.gg/emojis/9109_Sad_Cat_Thumbs_Up.png" width="30">
## Setting

### Current limitations:
- there is currently only one task
- each station has only one topic
- stations give static trust ratings to train creator
### current settings
- train user: have one project consisting out of a topic a task