open System
open System.Text
open System.Text.RegularExpressions
open Microsoft.FSharp.Reflection
open System.IO

// Parse a PDB file atom record into tuples
let scan_pdb_atom_record ( s : String ) =
     let rec_name = s.Substring (0, 6)
     let atom_num = int ( s.Substring (6, 5) )
     let atom_name = s.Substring (12, 4)
     let alt_loc = s.[16]
     let res_name = s.Substring (17, 3)
     let chain_id = s.[21]
     let res_seq_num = int (s.Substring (22, 4))
     let res_ins_code = s.[26]
     let x = float (s.Substring (30,8) )
     let y = float (s.Substring (38,8) )
     let z = float (s.Substring (46,8) )
     let occupancy = float (s.Substring(54,6) )
     let temp_factor = float (s.Substring(60,6) )
     let seg_id = s.Substring(72,4)
     let element = s.Substring(76,2)
     let atom_charge = s.Substring(78,2)
     (rec_name, atom_num, atom_name, alt_loc, res_name, chain_id, res_seq_num, 
      res_ins_code, x, y, z, occupancy, temp_factor, seg_id, element, atom_charge )

let readlines filepath = System.IO.File.ReadLines(filepath)

let PDB_lines_1 = readlines "C:\\Users\\andrewdodge16\\Desktop\\1YR2.pdb" 
                  |>Seq.filter(fun s -> s.StartsWith("ATOM"))
                  |>Seq.map(fun s -> scan_pdb_atom_record s)

let PDB_lines_2 = PDB_lines_1
                  |>Seq.groupBy(fun (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) -> (g,e)  )

let PDB_lines   = PDB_lines_2 
                  |>Seq.map(fun ((a,b),c) -> 
                                        let ln = float(Seq.length c)
                                        let (sx,sy,sz) = c|>Seq.fold (fun (xs,ys,zs) (a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) -> (xs+i,ys+j,zs+k) ) (0.0,0.0,0.0)
                                        ((a,b),(sx/ln,sy/ln,sz/ln))
                          )
                |>Seq.toList

let pairDistance (x1,y1,z1) (x2,y2,z2) = 
    Math.Sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2))

let rec FindClosestPairs acc seq =
            match seq with
            |[]->acc
            |head::tail -> 
                           let tacc = 
                                       tail
                                       |>List.map(fun x -> (head, x))
                                       //|>List.filter(fun ( (a,b), (c,d) ) -> ( pairDistance b d ) <= 17.5 )
                                       |>List.map(fun ( (a,b), (c,d) ) -> ( (a,c),(pairDistance b d) ) )

                           FindClosestPairs (acc@tacc) tail
                            
let results = FindClosestPairs [] PDB_lines
              |>List.sortBy(fun ( (a,b), d ) -> d )
              |>List.map(fun ( (a,b), d ) -> sprintf "%A %A %f" a b d ) 
printfn "%A" results

File.WriteAllLines ("C:\\Users\\andrewdodge16\\Desktop\\TEST_ALL_PhysicalProximityResultsStanford.txt", results )

